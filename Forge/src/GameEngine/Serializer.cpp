#include "Fpch.h"
#include "Serializer.h"

void forge::Serializer::SerializeType( const rtti::Type& type, void* address )
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

	default:
		FORGE_FATAL();
	}
}

void forge::Serializer::SerializePrimitive( const rtti::Type& type, void* address )
{
	switch ( m_mode )
	{
	case forge::Serializer::Mode::Saving:
		m_stream.write( static_cast< const char* >( address ), type.GetSize() );
		break;

	case forge::Serializer::Mode::Loading:
		m_stream.read( static_cast< char* >( address ), type.GetSize() );
		break;

	default:
		FORGE_FATAL();
	}
}

// boost like
static constexpr rtti::ID CombineIds( rtti::ID first, rtti::ID second )
{
	return first ^ ( second + 0x9e3779b9 + ( first << 6 ) + ( first >> 2 ) );
}

void forge::Serializer::SerializeClassOrStruct( const rtti::Type& type, void* address )
{
	Uint32 propertiesAmount = static_cast< Uint32 >( type.GetPropertiesAmount() );

	switch ( m_mode )
	{
	case Mode::Saving:
	{
		m_stream.write( reinterpret_cast< const char* >( &propertiesAmount ), sizeof( propertiesAmount ) );
		for ( Uint32 i = 0u; i < propertiesAmount; ++i )
		{
			const ::rtti::Property* prop = type.GetProperty( i );
			
			rtti::ID propertyId = prop->GetID();
			m_stream.write( reinterpret_cast< const char* >( &propertyId ), sizeof( propertyId ) );
			
			rtti::ID typeId = prop->GetType().GetID();
			m_stream.write( reinterpret_cast< const char* >( &typeId ), sizeof( typeId ) );

			Uint64 serializedSize = 0u;
			auto pos = m_stream.tellp();
			m_stream.write( reinterpret_cast< const char* >( &serializedSize ), sizeof( serializedSize ) );
			SerializeType( prop->GetType(), prop->GetAddress( address ) );
			auto endPos = m_stream.tellp();
			serializedSize = m_stream.tellp() - pos - sizeof( serializedSize );

			m_stream.seekp( pos, std::ios_base::beg );
			m_stream.write( reinterpret_cast< const char* >( &serializedSize ), sizeof( serializedSize ) );

			m_stream.seekp( endPos, std::ios_base::beg );
		}
		break;
	}

	case Mode::Loading:
	{
		decltype( propertiesAmount ) serializedPropertiesAmount = 0u;
		m_stream.read( reinterpret_cast< char* >( &serializedPropertiesAmount ), sizeof( serializedPropertiesAmount ) );

		std::unordered_map< rtti::ID, std::streampos > m_serializedProperties;

		for ( Uint32 i = 0u; i < serializedPropertiesAmount; ++i )
		{
			rtti::ID propertyId = 0u;
			m_stream.read( reinterpret_cast< char* >( &propertyId ), sizeof( propertyId ) );

			rtti::ID typeId = 0u;
			m_stream.read( reinterpret_cast< char* >( &typeId ), sizeof( typeId ) );

			Uint64 serializedSize = 0u;
			m_stream.read( reinterpret_cast< char* >( &serializedSize ), sizeof( serializedSize ) );

			FORGE_ASSERT( !m_serializedProperties.contains( CombineIds( propertyId, typeId ) ) );
			m_serializedProperties[ CombineIds( propertyId, typeId ) ] = m_stream.tellg();

			m_stream.seekg( serializedSize, std::ios_base::cur );
		}

		auto endPos = m_stream.tellg();

		for ( Uint32 i = 0u; i < propertiesAmount; ++i )
		{
			const ::rtti::Property* prop = type.GetProperty( i );

			auto foundSerialized = m_serializedProperties.find( CombineIds( prop->GetID(), prop->GetType().GetID() ) );
			if ( foundSerialized != m_serializedProperties.end() )
			{
				m_stream.seekg( foundSerialized->second, std::ios_base::beg );
				SerializeType( prop->GetType(), prop->GetAddress( address ) );
			}
		}

		break;
	}

	default:
		FORGE_FATAL();
	}
}

void forge::Serializer::SerializeArray( const rtti::ContainerType& type, void* address )
{
	switch ( m_mode )
	{
	case forge::Serializer::Mode::Saving:
	{
		const Uint32 amount = static_cast< Uint32 >( type.GetElementsAmount( address ) );
		m_stream.write( reinterpret_cast< const char* >( &amount ), sizeof( amount ) );

		type.VisitElements( address, [ this, &type ]( const void* elementAddress )
			{
				SerializeType( type.GetInternalType(), const_cast< void* >( elementAddress ) );
			} );

		break;
	}

	case forge::Serializer::Mode::Loading:
	{
		Uint32 serializedAmount = 0u;
		m_stream.read( reinterpret_cast< char* >( &serializedAmount ), sizeof( serializedAmount ) );
		Uint32 arrayTypeCount = static_cast< Uint32 >( type.GetElementsAmount( nullptr ) );

		const Uint32 internalTypeSize = static_cast< Uint32 >( type.GetInternalType().GetSize() );

		for ( Uint32 i = 0u; i < Math::Min( serializedAmount, arrayTypeCount ); ++i )
		{
			Byte* currentAddress = static_cast< Byte* >( address ) + i * internalTypeSize;
			SerializeType( type.GetInternalType(), currentAddress );
		}

		break;
	}

	default:
		FORGE_FATAL();
	}
}

void forge::Serializer::SerializeDynamicContainer( const rtti::ContainerType& type, void* address )
{
	switch ( m_mode )
	{
	case forge::Serializer::Mode::Saving:
	{
		const Uint32 amount = static_cast< Uint32 >( type.GetElementsAmount( address ) );
		m_stream.write( reinterpret_cast< const char* >( &amount ), sizeof( amount ) );

		type.VisitElements( address, [ this, &type ]( const void* elementAddress )
			{
				SerializeType( type.GetInternalType(), const_cast< void* >( elementAddress ) );
			} );

		break;
	}

	case forge::Serializer::Mode::Loading:
	{
		Uint32 amount = 0u;
		m_stream.read( reinterpret_cast< char* >( &amount ), sizeof( amount ) );

		FORGE_ASSERT( dynamic_cast< const ::rtti::DynamicContainerType* >( &type ) );
		forge::RawSmartPtr buffer( type.GetInternalType().GetSize() );
		for ( Uint32 i = 0u; i < amount; ++i )
		{
			type.GetInternalType().ConstructInPlace( buffer.GetData() );
			SerializeType( type.GetInternalType(), buffer.GetData() );
			static_cast< const ::rtti::DynamicContainerType* >( &type )->EmplaceElement( address, buffer.GetData() );
		}

		break;
	}

	default:
		FORGE_FATAL();
	}
}

void forge::Serializer::SerializeUniquePointer( const rtti::UniquePtrBaseType& type, void* address )
{
	switch ( m_mode )
	{
	case forge::Serializer::Mode::Saving:
	{
		void* pointedAddress = type.GetPointedAddress( address );
		SerializeType( type.GetInternalType(), pointedAddress );
		break;
	}

	case forge::Serializer::Mode::Loading:
	{
		type.ConstructInPlace( address );
		forge::RawSmartPtr buffer( type.GetInternalType().GetSize() );
		type.GetInternalType().ConstructInPlace( buffer.GetData() );
		SerializeType( type.GetInternalType(), buffer.GetData() );
		type.SetPointedAddress( address, buffer.GetData() );
		buffer.Release();
		break;
	}

	default:
		FORGE_FATAL();
	}
}
