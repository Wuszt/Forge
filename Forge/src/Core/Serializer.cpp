#include "Fpch.h"
#include "Serializer.h"
#include "Streams.h"

void forge::Serializer::SerializeType( const rtti::Type& type, const void* address )
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

void forge::Deserializer::DeserializeType( const rtti::Type& type, void* address )
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
	Uint32 propertiesAmount = static_cast< Uint32 >( type.GetPropertiesAmount() );

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
}

void forge::Deserializer::DeserializeClassOrStruct( const rtti::Type& type, void* address )
{
	Uint32 propertiesAmount = static_cast< Uint32 >( type.GetPropertiesAmount() );
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
			DeserializeType( prop->GetType(), prop->GetAddress( address ) );
		}
	}
}

void forge::Serializer::SerializeArray( const rtti::ContainerType& type, const void* address )
{
	m_stream.Write( static_cast< Uint32 >( type.GetElementsAmount( address ) ) );

	type.VisitElements( address, [ this, &type ]( const void* elementAddress )
		{
			SerializeType( type.GetInternalType(), elementAddress );
		} );
}

void forge::Deserializer::DeserializeArray( const rtti::ContainerType& type, void* address )
{
	Uint32 serializedAmount = m_stream.Read< Uint32 >();
	Uint32 arrayTypeCount = static_cast< Uint32 >( type.GetElementsAmount( nullptr ) );

	const Uint32 internalTypeSize = static_cast< Uint32 >( type.GetInternalType().GetSize() );

	Uint32 deserializeAmount = serializedAmount < arrayTypeCount ? serializedAmount : arrayTypeCount;

	for ( Uint32 i = 0u; i < deserializeAmount; ++i )
	{
		Byte* currentAddress = static_cast< Byte* >( address ) + i * internalTypeSize;
		DeserializeType( type.GetInternalType(), currentAddress );
	}
}

void forge::Serializer::SerializeDynamicContainer( const rtti::ContainerType& type, const void* address )
{
	m_stream.Write( static_cast< Uint32 >( type.GetElementsAmount( address ) ) );

	type.VisitElements( address, [ this, &type ]( const void* elementAddress )
		{
			SerializeType( type.GetInternalType(), const_cast< void* >( elementAddress ) );
		} );
}

void forge::Deserializer::DeserializeDynamicContainer( const rtti::ContainerType& type, void* address )
{
	Uint32 amount = m_stream.Read< Uint32 >();

	FORGE_ASSERT( dynamic_cast< const ::rtti::DynamicContainerType* >( &type ) );
	forge::RawSmartPtr buffer( type.GetInternalType().GetSize() );
	for ( Uint32 i = 0u; i < amount; ++i )
	{
		type.GetInternalType().ConstructInPlace( buffer.GetData() );
		DeserializeType( type.GetInternalType(), buffer.GetData() );
		static_cast< const ::rtti::DynamicContainerType* >( &type )->EmplaceElement( address, buffer.GetData() );
	}
}

void forge::Serializer::SerializeUniquePointer( const rtti::UniquePtrBaseType& type, const void* address )
{
	const void* pointedAddress = type.GetPointedAddress( address );
	SerializeType( type.GetInternalType(), pointedAddress );
}

void forge::Deserializer::DeserializeUniquePointer( const rtti::UniquePtrBaseType& type, void* address )
{
	type.ConstructInPlace( address );
	forge::RawSmartPtr buffer( type.GetInternalType().GetSize() );
	type.GetInternalType().ConstructInPlace( buffer.GetData() );
	DeserializeType( type.GetInternalType(), buffer.GetData() );
	type.SetPointedAddress( address, buffer.GetData() );
	buffer.Release();
}
