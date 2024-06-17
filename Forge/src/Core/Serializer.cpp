#include "Fpch.h"
#include "Serializer.h"
#include "Streams.h"

void forge::Serializer::Serialize( const rtti::Type& type, const void* address )
{
	switch ( type.GetKind() )
	{
	case rtti::Type::Kind::Class:
	case rtti::Type::Kind::Struct:
	case rtti::Type::Kind::RuntimeType:
		SerializeClassOrStruct( type, address );
		break;

	case rtti::Type::Kind::Primitive:
		SerializePrimitive( type, address );
		break;

	case rtti::Type::Kind::Array:
		SerializeArray( static_cast< const ::rtti::ContainerType& >( type ), address );
		break;

	case rtti::Type::Kind::Vector:
	case rtti::Type::Kind::Set:
	case rtti::Type::Kind::Map:
		SerializeDynamicContainer( static_cast< const ::rtti::ContainerType& >( type ), address );
		break;

	case rtti::Type::Kind::UniquePointer:
		SerializeUniquePointer( static_cast< const ::rtti::UniquePtrBaseType& >( type ), address );
		break;

	case rtti::Type::Kind::SharedPointer:
		SerializeSharedPointer( static_cast< const ::rtti::SharedPtrBaseType& >( type ), address );
		break;

	default:
		FORGE_FATAL();
	}
}

void forge::Deserializer::Deserialize( const rtti::Type& type, void* address )
{
	switch ( type.GetKind() )
	{
	case rtti::Type::Kind::Class:
	case rtti::Type::Kind::Struct:
	case rtti::Type::Kind::RuntimeType:
		DeserializeClassOrStruct( type, address );
		break;

	case rtti::Type::Kind::Primitive:
		DeserializePrimitive( type, address );
		break;

	case rtti::Type::Kind::Array:
		DeserializeArray( static_cast< const ::rtti::ContainerType& >( type ), address );
		break;

	case rtti::Type::Kind::Vector:
	case rtti::Type::Kind::Set:
	case rtti::Type::Kind::Map:
		DeserializeDynamicContainer( static_cast< const ::rtti::ContainerType& >( type ), address );
		break;

	case rtti::Type::Kind::UniquePointer:
		DeserializeUniquePointer( static_cast< const ::rtti::UniquePtrBaseType& >( type ), address );
		break;

	case rtti::Type::Kind::SharedPointer:
		DeserializeSharedPointer( static_cast< const ::rtti::SharedPtrBaseType& >( type ), address );
		break;

	default:
		FORGE_FATAL();
	}
}

void forge::Serializer::SerializePrimitive( const rtti::Type& type, const void* address )
{
	m_stream.Write( address, type.GetSize() );
}

void forge::Deserializer::DeserializePrimitive( const rtti::Type& type, void* address )
{
	m_stream.Read( address, type.GetSize() );
}

// boost like
static constexpr rtti::ID CombineIds( rtti::ID first, rtti::ID second )
{
	return first ^ ( second + 0x9e3779b9 + ( first << 6 ) + ( first >> 2 ) );
}

void forge::Serializer::SerializeClassOrStruct( const rtti::Type& type, const void* address )
{
	if ( const rtti::Function* func = type.FindMethod( "Serialize" ) )
	{
		if ( func->GetParametersAmount() == 1 )
		{
			const auto* parameter = func->GetParameterTypeDesc( 0 );
			FORGE_ASSERT( parameter );
			if ( parameter->GetType().IsA< forge::Stream >() )
			{
				func->Call( const_cast< void* >( address ), &m_stream, nullptr );
				return;
			}
		}
	}

	Uint32 propertiesAmount = static_cast< Uint32 >( type.GetPropertiesAmount() );

	m_stream.Write( propertiesAmount );
	for ( Uint32 i = 0u; i < propertiesAmount; ++i )
	{
		const ::rtti::Property* prop = type.GetProperty( i );

		m_stream.Write( prop->GetID() );
		m_stream.Write( prop->GetType().GetID() );

		Uint64 serializedSize = 0u;
		auto pos = m_stream.GetPos();
		m_stream.Write( serializedSize );
		Serialize( prop->GetType(), prop->GetAddress( address ) );
		auto endPos = m_stream.GetPos();
		serializedSize = m_stream.GetPos() - pos - sizeof( serializedSize );

		m_stream.SetPos( pos );
		m_stream.Write( serializedSize );

		m_stream.SetPos( endPos );
	}
}

void forge::Deserializer::DeserializeClassOrStruct( const rtti::Type& type, void* address )
{
	if ( const rtti::Function* func = type.FindMethod( "Deserialize" ) )
	{
		if ( func->GetParametersAmount() == 1 )
		{
			const auto* parameter = func->GetParameterTypeDesc( 0 );
			FORGE_ASSERT( parameter );
			if ( parameter->GetType().IsA< forge::Stream >() )
			{
				func->Call( const_cast< void* >( address ), &m_stream, nullptr );
				return;
			}
		}
	}

	Uint32 propertiesAmount = static_cast< Uint32 >( type.GetPropertiesAmount() );
	Uint32 serializedPropertiesAmount = m_stream.Read< Uint32 >();

	std::unordered_map< rtti::ID, std::streampos > m_serializedProperties;

	for ( Uint32 i = 0u; i < serializedPropertiesAmount; ++i )
	{
		rtti::ID propertyId = m_stream.Read< rtti::ID >();
		rtti::ID typeId = m_stream.Read< rtti::ID >();
		Uint64 serializedSize = m_stream.Read< Uint64 >();

		FORGE_ASSERT( !m_serializedProperties.contains( CombineIds( propertyId, typeId ) ) );
		m_serializedProperties[ CombineIds( propertyId, typeId ) ] = m_stream.GetPos();

		m_stream.SetPos( m_stream.GetPos() + serializedSize );
	}

	auto endPos = m_stream.GetPos();

	for ( Uint32 i = 0u; i < propertiesAmount; ++i )
	{
		const ::rtti::Property* prop = type.GetProperty( i );

		auto foundSerialized = m_serializedProperties.find( CombineIds( prop->GetID(), prop->GetType().GetID() ) );
		if ( foundSerialized != m_serializedProperties.end() )
		{
			m_stream.SetPos( foundSerialized->second );
			Deserialize( prop->GetType(), prop->GetAddress( address ) );
		}
	}
}

void forge::Serializer::SerializeArray( const rtti::ContainerType& type, const void* address )
{
	m_stream.Write( static_cast< Uint32 >( type.GetElementsAmount( address ) ) );

	type.VisitElementsAsProperties( address, [ this, &type, address ]( const rtti::Property& property )
		{
			Serialize( type.GetInternalTypeDesc().GetType(), property.GetAddress( address ) );
			return ::rtti::VisitOutcome::Continue;
		} );
}

void forge::Deserializer::DeserializeArray( const rtti::ContainerType& type, void* address )
{
	Uint32 serializedAmount = m_stream.Read< Uint32 >();
	Uint32 arrayTypeCount = static_cast< Uint32 >( type.GetElementsAmount( nullptr ) );

	const Uint32 internalTypeSize = static_cast< Uint32 >( type.GetInternalTypeDesc().GetType().GetSize() );

	Uint32 deserializeAmount = serializedAmount < arrayTypeCount ? serializedAmount : arrayTypeCount;

	for ( Uint32 i = 0u; i < deserializeAmount; ++i )
	{
		Byte* currentAddress = static_cast< Byte* >( address ) + i * internalTypeSize;
		Deserialize( type.GetInternalTypeDesc().GetType(), currentAddress );
	}
}

void forge::Serializer::SerializeDynamicContainer( const rtti::ContainerType& type, const void* address )
{
	m_stream.Write( static_cast< Uint32 >( type.GetElementsAmount( address ) ) );

	type.VisitElementsAsProperties( address, [ this, &type, address ]( const rtti::Property& property )
		{
			Serialize( type.GetInternalTypeDesc().GetType(), property.GetAddress( address ) );
			return ::rtti::VisitOutcome::Continue;
		} );
}

void forge::Deserializer::DeserializeDynamicContainer( const rtti::ContainerType& type, void* address )
{
	Uint32 amount = m_stream.Read< Uint32 >();

	FORGE_ASSERT( dynamic_cast< const ::rtti::DynamicContainerType* >( &type ) );
	forge::UniqueRawPtr buffer( type.GetInternalTypeDesc().GetType().GetSize() );
	for ( Uint32 i = 0u; i < amount; ++i )
	{
		type.GetInternalTypeDesc().GetType().ConstructInPlace( buffer.GetData() );
		Deserialize( type.GetInternalTypeDesc().GetType(), buffer.GetData() );
		static_cast< const ::rtti::DynamicContainerType* >( &type )->EmplaceElement( address, buffer.GetData() );
	}
}

void forge::Serializer::SerializeUniquePointer( const rtti::UniquePtrBaseType& type, const void* address )
{
	const void* pointedAddress = type.GetPointedAddress( address );
	Serialize( type.GetInternalTypeDesc().GetType(), pointedAddress );
}

void forge::Deserializer::DeserializeUniquePointer( const rtti::UniquePtrBaseType& type, void* address )
{
	type.ConstructInPlace( address );
	forge::UniqueRawPtr buffer( type.GetInternalTypeDesc().GetType().GetSize() );
	type.GetInternalTypeDesc().GetType().ConstructInPlace( buffer.GetData() );
	Deserialize( type.GetInternalTypeDesc().GetType(), buffer.GetData() );
	type.SetPointedAddress( address, buffer.GetData() );
	buffer.Release();
}

void forge::Serializer::SerializeSharedPointer( const rtti::SharedPtrBaseType& type, const void* address )
{
	const void* pointedAddress = type.GetPointedAddress( address );
	auto found = m_sharedPtrsOffsets.find( pointedAddress );
	if ( found != m_sharedPtrsOffsets.end() )
	{
		m_stream.Write( found->second );
	}
	else
	{
		m_stream.Write( std::numeric_limits< Uint64 >::max() );
		m_sharedPtrsOffsets[ pointedAddress ] = m_stream.GetPos();
		Serialize( type.GetInternalTypeDesc().GetType(), pointedAddress );
	}
}

void forge::Deserializer::DeserializeSharedPointer( const rtti::SharedPtrBaseType& type, void* address )
{
	type.ConstructInPlace( address );
	const Uint64 dataPos = m_stream.Read< Uint64 >();
	auto deserializeInternalFunc = [&]()
	{
		auto currentPos = m_stream.GetPos();
		forge::UniqueRawPtr buffer( type.GetInternalTypeDesc().GetType().GetSize() );
		type.GetInternalTypeDesc().GetType().ConstructInPlace( buffer.GetData() );
		Deserialize( type.GetInternalTypeDesc().GetType(), buffer.GetData() );
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
			auto currentPos = m_stream.GetPos();
			m_stream.SetPos( dataPos );
			deserializeInternalFunc();
			m_stream.SetPos( currentPos );
		}
	}


}