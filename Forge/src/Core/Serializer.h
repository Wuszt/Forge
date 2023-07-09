#pragma once

namespace forge
{
	class Stream;

	class Serializer
	{
	public:
		Serializer( Stream& stream )
			: m_stream( stream )
		{}

		template< class T >
		void Serialize( const T& data )
		{
			SerializeType( ::rtti::GetTypeInstanceOf< T >(), &data );
		}

	private:
		void SerializeType( const rtti::Type& type, const void* address );
		void SerializePrimitive( const rtti::Type& type, const void* address );
		void SerializeClassOrStruct( const rtti::Type& type, const void* address );
		void SerializeArray( const rtti::ContainerType& type, const void* address );
		void SerializeDynamicContainer( const rtti::ContainerType& type, const void* address );
		void SerializeUniquePointer( const rtti::UniquePtrBaseType& type, const void* address );

		Stream& m_stream;
	};

	class Deserializer
	{
	public:
		Deserializer( Stream& stream )
			: m_stream( stream )
		{}

		template< class T >
		void Deserialize( T& data )
		{
			DeserializeType( ::rtti::GetTypeInstanceOf< T >(), &data );
		}

	private:
		void DeserializeType( const rtti::Type& type, void* address );
		void DeserializePrimitive( const rtti::Type& type, void* address );
		void DeserializeClassOrStruct( const rtti::Type& type, void* address );
		void DeserializeArray( const rtti::ContainerType& type, void* address );
		void DeserializeDynamicContainer( const rtti::ContainerType& type, void* address );
		void DeserializeUniquePointer( const rtti::UniquePtrBaseType& type, void* address );

		Stream& m_stream;
	};
}

