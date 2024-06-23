#include "Fpch.h"
#include "Serializer.h"
#include "Streams.h"

RTTI_IMPLEMENT_TYPE( forge::Serializer );
RTTI_IMPLEMENT_TYPE( forge::Deserializer );

void forge::Serializer::Serialize( const void* address, const rtti::Type& type )
{
	switch ( type.GetKind() )
	{
	case rtti::Type::Kind::Class:
	case rtti::Type::Kind::Struct:
	case rtti::Type::Kind::RuntimeType:
		SerializeClassOrStruct( address, type );
		break;

	case rtti::Type::Kind::Primitive:
		SerializePrimitive( address, type );
		break;

	case rtti::Type::Kind::Array:
		SerializeArray( address, static_cast< const ::rtti::ContainerType& >( type ) );
		break;

	case rtti::Type::Kind::Vector:
	case rtti::Type::Kind::Set:
	case rtti::Type::Kind::Map:
		SerializeDynamicContainer( address, static_cast< const ::rtti::ContainerType& >( type ) );
		break;

	case rtti::Type::Kind::UniquePointer:
		SerializeUniquePointer( address, static_cast< const ::rtti::UniquePtrBaseType& >( type ) );
		break;

	case rtti::Type::Kind::SharedPointer:
		SerializeSharedPointer( address, static_cast< const ::rtti::SharedPtrBaseType& >( type ) );
		break;

	case rtti::Type::Kind::String:
		SerializeString( address, static_cast< const ::rtti::StringType& >( type ) );
		break;

	default:
		FORGE_FATAL();
	}
}

void forge::Serializer::Serialize( const void* address, Uint64 size )
{
	m_stream->Write( address, size );
}

void forge::Deserializer::Deserialize( void* address, const rtti::Type& type )
{
	switch ( type.GetKind() )
	{
	case rtti::Type::Kind::Class:
	case rtti::Type::Kind::Struct:
	case rtti::Type::Kind::RuntimeType:
		DeserializeClassOrStruct( address, type );
		break;

	case rtti::Type::Kind::Primitive:
		DeserializePrimitive( address, type );
		break;

	case rtti::Type::Kind::Array:
		DeserializeArray( address, static_cast< const ::rtti::ContainerType& >( type ) );
		break;

	case rtti::Type::Kind::Vector:
	case rtti::Type::Kind::Set:
	case rtti::Type::Kind::Map:
		DeserializeDynamicContainer( address, static_cast< const ::rtti::ContainerType& >( type ) );
		break;

	case rtti::Type::Kind::UniquePointer:
		DeserializeUniquePointer( address, static_cast< const ::rtti::UniquePtrBaseType& >( type ) );
		break;

	case rtti::Type::Kind::SharedPointer:
		DeserializeSharedPointer( address, static_cast< const ::rtti::SharedPtrBaseType& >( type ) );
		break;

	case rtti::Type::Kind::String:
		DeserializeString( address, static_cast< const ::rtti::StringType& >( type ) );
		break;

	default:
		FORGE_FATAL();
	}
}

void forge::Deserializer::Deserialize( void* address, Uint64 size )
{
	m_stream->Read( address, size );
}

void forge::Serializer::SerializePrimitive( const void* address, const rtti::Type& type )
{
	m_stream->Write( address, type.GetSize() );
}

void forge::Serializer::SerializeString( const void* address, const rtti::StringType& type )
{
	const std::string* str = static_cast< const std::string* >( address );
	m_stream->Write( str->data(), str->size() );
	m_stream->Write( Char( 0 ) );
}

void forge::Deserializer::DeserializePrimitive( void* address, const rtti::Type& type )
{
	m_stream->Read( address, type.GetSize() );
}

void forge::Deserializer::DeserializeString( void* address, const rtti::StringType& type )
{
	type.ConstructInPlace( address );
	std::string* str = static_cast< std::string* >( address );
	std::vector< Char > buffer;
	buffer.reserve( 25 );
	Uint32 readStep = 25;
	while ( true )
	{
		const Uint64 remainingDataSize = m_stream->GetSize() - m_stream->GetPos();
		const Uint64 readSize = readStep < remainingDataSize ? readStep : remainingDataSize;
		readStep *= 2;

		const Uint64 currentDataSize = buffer.size();
		buffer.resize( currentDataSize + readSize );
		m_stream->Read( buffer.data() + currentDataSize, readSize );
		for ( Uint64 i = currentDataSize; i < buffer.size(); ++i )
		{
			if ( buffer[ i ] == Char( 0 ) )
			{
				m_stream->SetPos( m_stream->GetPos() - ( readSize - ( i - currentDataSize ) ) + 1 );
				str->append( buffer.data() );
				return;
			}
		}
	}
}

// boost like
static constexpr rtti::ID CombineIds( rtti::ID first, rtti::ID second )
{
	return first ^ ( second + 0x9e3779b9 + ( first << 6 ) + ( first >> 2 ) );
}

void forge::Serializer::SerializeClassOrStruct( const void* address, const rtti::Type& type )
{
	if ( const rtti::Function* func = type.FindMethod( "Serialize" ) )
	{
		if ( func->GetParametersAmount() == 1 )
		{
			const auto* parameter = func->GetParameterTypeDesc( 0 );
			FORGE_ASSERT( parameter );
			if ( parameter->GetType().IsA< forge::Serializer >() )
			{
				func->Call( const_cast< void* >( address ), this, nullptr );
				return;
			}
		}
	}

	Uint32 propertiesAmount = static_cast< Uint32 >( type.GetPropertiesAmount() );

	m_stream->Write( propertiesAmount );
	for ( Uint32 i = 0u; i < propertiesAmount; ++i )
	{
		const ::rtti::Property* prop = type.GetProperty( i );

		m_stream->Write( prop->GetID() );
		m_stream->Write( prop->GetType().GetID() );

		Uint64 serializedSize = 0u;
		auto pos = m_stream->GetPos();
		m_stream->Write( serializedSize );
		Serialize( prop->GetAddress( address ), prop->GetType() );
		auto endPos = m_stream->GetPos();
		serializedSize = m_stream->GetPos() - pos - sizeof( serializedSize );

		m_stream->SetPos( pos );
		m_stream->Write( serializedSize );

		m_stream->SetPos( endPos );
	}
}

void forge::Deserializer::DeserializeClassOrStruct( void* address, const rtti::Type& type )
{
	type.ConstructInPlace( address );
	if ( const rtti::Function* func = type.FindMethod( "Deserialize" ) )
	{
		if ( func->GetParametersAmount() == 1 )
		{
			const auto* parameter = func->GetParameterTypeDesc( 0 );
			FORGE_ASSERT( parameter );
			if ( parameter->GetType().IsA< forge::Deserializer >() )
			{
				func->Call( const_cast< void* >( address ), this, nullptr );
				return;
			}
		}
	}

	Uint32 propertiesAmount = static_cast< Uint32 >( type.GetPropertiesAmount() );
	Uint32 serializedPropertiesAmount = m_stream->Read< Uint32 >();

	std::unordered_map< rtti::ID, std::streampos > m_serializedProperties;

	for ( Uint32 i = 0u; i < serializedPropertiesAmount; ++i )
	{
		rtti::ID propertyId = m_stream->Read< rtti::ID >();
		rtti::ID typeId = m_stream->Read< rtti::ID >();
		Uint64 serializedSize = m_stream->Read< Uint64 >();

		FORGE_ASSERT( !m_serializedProperties.contains( CombineIds( propertyId, typeId ) ) );
		m_serializedProperties[ CombineIds( propertyId, typeId ) ] = m_stream->GetPos();

		m_stream->SetPos( m_stream->GetPos() + serializedSize );
	}

	auto endPos = m_stream->GetPos();

	for ( Uint32 i = 0u; i < propertiesAmount; ++i )
	{
		const ::rtti::Property* prop = type.GetProperty( i );

		auto foundSerialized = m_serializedProperties.find( CombineIds( prop->GetID(), prop->GetType().GetID() ) );
		if ( foundSerialized != m_serializedProperties.end() )
		{
			m_stream->SetPos( foundSerialized->second );
			Deserialize( prop->GetAddress( address ), prop->GetType() );
		}
	}
}

void forge::Serializer::SerializeArray( const void* address, const rtti::ContainerType& type )
{
	m_stream->Write( static_cast< Uint32 >( type.GetElementsAmount( address ) ) );

	type.VisitElementsAsProperties( address, [ this, &type, address ]( const rtti::Property& property )
		{
			Serialize( property.GetAddress( address ), type.GetInternalTypeDesc().GetType() );
			return ::rtti::VisitOutcome::Continue;
		} );
}

void forge::Deserializer::DeserializeArray( void* address, const rtti::ContainerType& type )
{
	type.ConstructInPlace( address );
	const Uint32 serializedAmount = m_stream->Read< Uint32 >();
	const Uint32 arrayTypeCount = static_cast< Uint32 >( type.GetElementsAmount( nullptr ) );

	const Uint32 internalTypeSize = static_cast< Uint32 >( type.GetInternalTypeDesc().GetType().GetSize() );

	Uint32 deserializeAmount = serializedAmount < arrayTypeCount ? serializedAmount : arrayTypeCount;

	for ( Uint32 i = 0u; i < deserializeAmount; ++i )
	{
		Byte* currentAddress = static_cast< Byte* >( address ) + i * internalTypeSize;
		Deserialize( currentAddress, type.GetInternalTypeDesc().GetType() );
	}
}

void forge::Serializer::SerializeDynamicContainer( const void* address, const rtti::ContainerType& type )
{
	m_stream->Write( static_cast< Uint32 >( type.GetElementsAmount( address ) ) );

	type.VisitElementsAsProperties( address, [ this, &type, address ]( const rtti::Property& property )
		{
			Serialize( property.GetAddress( address ), type.GetInternalTypeDesc().GetType() );
			return ::rtti::VisitOutcome::Continue;
		} );
}

void forge::Deserializer::DeserializeDynamicContainer( void* address, const rtti::ContainerType& type )
{
	type.ConstructInPlace( address );
	Uint32 amount = m_stream->Read< Uint32 >();

	FORGE_ASSERT( dynamic_cast< const ::rtti::DynamicContainerType* >( &type ) );
	forge::UniqueRawPtr buffer( type.GetInternalTypeDesc().GetType().GetSize() );
	for ( Uint32 i = 0u; i < amount; ++i )
	{
		Deserialize( buffer.GetData(), type.GetInternalTypeDesc().GetType() );
		static_cast< const ::rtti::DynamicContainerType* >( &type )->EmplaceElement( address, buffer.GetData() );
	}
}

void forge::Serializer::SerializeUniquePointer( const void* address, const rtti::UniquePtrBaseType& type )
{
	const void* pointedAddress = type.GetPointedAddress( address );
	Serialize( pointedAddress, type.GetInternalTypeDesc().GetType() );
}

void forge::Deserializer::DeserializeUniquePointer( void* address, const rtti::UniquePtrBaseType& type )
{
	type.ConstructInPlace( address );
	forge::UniqueRawPtr buffer( type.GetInternalTypeDesc().GetType().GetSize() );
	Deserialize( buffer.GetData(), type.GetInternalTypeDesc().GetType() );
	type.SetPointedAddress( address, buffer.GetData() );
	buffer.Release();
}

void forge::Serializer::SerializeSharedPointer( const void* address, const rtti::SharedPtrBaseType& type )
{
	const void* pointedAddress = type.GetPointedAddress( address );
	auto found = m_sharedPtrsOffsets.find( pointedAddress );
	if ( found != m_sharedPtrsOffsets.end() )
	{
		m_stream->Write( found->second );
	}
	else
	{
		m_stream->Write( std::numeric_limits< Uint64 >::max() );
		m_sharedPtrsOffsets[ pointedAddress ] = m_stream->GetPos();
		Serialize( pointedAddress, type.GetInternalTypeDesc().GetType() );
	}
}

void forge::Deserializer::DeserializeSharedPointer( void* address, const rtti::SharedPtrBaseType& type )
{
	type.ConstructInPlace( address );
	const Uint64 dataPos = m_stream->Read< Uint64 >();
	auto deserializeInternalFunc = [&]()
	{
		auto currentPos = m_stream->GetPos();
		forge::UniqueRawPtr buffer( type.GetInternalTypeDesc().GetType().GetSize() );
		Deserialize( buffer.GetData(), type.GetInternalTypeDesc().GetType() );
		type.SetPointedAddress( address, buffer.GetData() );
		m_sharedPtrsAddresses[ currentPos ] = address;
		buffer.Release();
	};

	if ( dataPos == std::numeric_limits< Uint64 >::max() )
	{
		deserializeInternalFunc();
	}
	else
	{
		auto found = m_sharedPtrsAddresses.find( dataPos );
		if ( found != m_sharedPtrsAddresses.end() )
		{
			type.AssignSharedPtr( found->second, address );
		}
		else
		{
			auto currentPos = m_stream->GetPos();
			m_stream->SetPos( dataPos );
			deserializeInternalFunc();
			m_stream->SetPos( currentPos );
		}
	}
}