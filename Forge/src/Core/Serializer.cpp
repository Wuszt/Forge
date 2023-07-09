#include "Fpch.h"
#include "Serializer.h"
#include "Streams.h"

forge::Serializer::Serializer( Mode mode, Stream& stream )
	: m_mode( mode )
	, m_stream( stream )
{}

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
		m_stream.Write( address, type.GetSize() );
		break;

	case forge::Serializer::Mode::Loading:
		m_stream.Read( address, type.GetSize() );
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
		m_stream.Write( propertiesAmount );
		for ( Uint32 i = 0u; i < propertiesAmount; ++i )
		{
			const ::rtti::Property* prop = type.GetProperty( i );
			
			m_stream.Write( prop->GetID() );
			m_stream.Write( prop->GetType().GetID() );

			Uint64 serializedSize = 0u;
			auto pos = m_stream.GetWritePos();
			m_stream.Write( serializedSize );

			SerializeType( prop->GetType(), prop->GetAddress( address ) );
			auto endPos = m_stream.GetWritePos();
			serializedSize = m_stream.GetWritePos() - pos - sizeof( serializedSize );

			m_stream.SetWritePos( pos );
			m_stream.Write( serializedSize );

			m_stream.SetWritePos( endPos );
		}
		break;
	}

	case Mode::Loading:
	{
		Uint32 serializedPropertiesAmount = m_stream.Read< Uint32 >();

		std::unordered_map< rtti::ID, std::streampos > m_serializedProperties;

		for ( Uint32 i = 0u; i < serializedPropertiesAmount; ++i )
		{
			rtti::ID propertyId = m_stream.Read< rtti::ID >();
			rtti::ID typeId = m_stream.Read< rtti::ID >();	
			Uint64 serializedSize = m_stream.Read< Uint64 >();

			FORGE_ASSERT( !m_serializedProperties.contains( CombineIds( propertyId, typeId ) ) );
			m_serializedProperties[ CombineIds( propertyId, typeId ) ] = m_stream.GetReadPos();

			m_stream.SetReadPos( m_stream.GetReadPos() + serializedSize );
		}

		auto endPos = m_stream.GetReadPos();

		for ( Uint32 i = 0u; i < propertiesAmount; ++i )
		{
			const ::rtti::Property* prop = type.GetProperty( i );

			auto foundSerialized = m_serializedProperties.find( CombineIds( prop->GetID(), prop->GetType().GetID() ) );
			if ( foundSerialized != m_serializedProperties.end() )
			{
				m_stream.SetReadPos( foundSerialized->second );
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
		m_stream.Write( static_cast< Uint32 >( type.GetElementsAmount( address ) ) );

		type.VisitElements( address, [ this, &type ]( const void* elementAddress )
			{
				SerializeType( type.GetInternalType(), const_cast< void* >( elementAddress ) );
			} );

		break;
	}

	case forge::Serializer::Mode::Loading:
	{
		Uint32 serializedAmount = m_stream.Read< Uint32 >();
		Uint32 arrayTypeCount = static_cast< Uint32 >( type.GetElementsAmount( nullptr ) );

		const Uint32 internalTypeSize = static_cast< Uint32 >( type.GetInternalType().GetSize() );

		Uint32 deserializeAmount = serializedAmount < arrayTypeCount ? serializedAmount : arrayTypeCount;

		for ( Uint32 i = 0u; i < deserializeAmount; ++i )
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
		m_stream.Write( static_cast< Uint32 >( type.GetElementsAmount( address ) ) );

		type.VisitElements( address, [ this, &type ]( const void* elementAddress )
			{
				SerializeType( type.GetInternalType(), const_cast< void* >( elementAddress ) );
			} );

		break;
	}

	case forge::Serializer::Mode::Loading:
	{
		Uint32 amount = m_stream.Read< Uint32 >();

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
