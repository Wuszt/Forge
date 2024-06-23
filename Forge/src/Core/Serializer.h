#pragma once

namespace forge
{
	class Stream;

	class Serializer
	{
		RTTI_DECLARE_CLASS( Serializer );

	public:
		Serializer( Stream& stream )
			: m_stream( &stream )
		{}

		template< class T >
		void Serialize( const T& data )
		{
			Serialize( &data, ::rtti::GetTypeInstanceOf< T >() );
		}

		void Serialize( const void* address, Uint64 size );
		void Serialize( const void* address, const rtti::Type& type );

	private:
		Serializer() = default;

		void SerializePrimitive( const void* address, const rtti::Type& type );
		void SerializeString( const void* address, const rtti::StringType& type );
		void SerializeClassOrStruct( const void* address, const rtti::Type& type );
		void SerializeArray( const void* address, const rtti::ContainerType& type );
		void SerializeDynamicContainer( const void* address, const rtti::ContainerType& type );
		void SerializeUniquePointer( const void* address, const rtti::UniquePtrBaseType& type );
		void SerializeSharedPointer( const void* address, const rtti::SharedPtrBaseType& type );

		std::unordered_map< const void*, Uint64 > m_sharedPtrsOffsets;
		Stream* m_stream = nullptr;
	};

	class Deserializer
	{
		RTTI_DECLARE_CLASS( Deserializer );

	public:
		Deserializer( Stream& stream )
			: m_stream( &stream )
		{}

		template< class T >
		void Deserialize( T& data )
		{
			Deserialize( &data, ::rtti::GetTypeInstanceOf< T >() );
		}

		template< class T >
		T Deserialize()
		{
			T result;
			Deserialize( result );
			return result;
		}

		void Deserialize( void* address, Uint64 size );
		void Deserialize( void* address, const rtti::Type& type );

	private:
		Deserializer() = default;

		void DeserializePrimitive( void* address, const rtti::Type& type );
		void DeserializeString( void* address, const rtti::StringType& type );
		void DeserializeClassOrStruct( void* address, const rtti::Type& type );
		void DeserializeArray( void* address, const rtti::ContainerType& type );
		void DeserializeDynamicContainer( void* address, const rtti::ContainerType& type );
		void DeserializeUniquePointer( void* address, const rtti::UniquePtrBaseType& type );
		void DeserializeSharedPointer( void* address, const rtti::SharedPtrBaseType& type );

		std::unordered_map< Uint64, void* > m_sharedPtrsAddresses;
		Stream* m_stream = nullptr;
	};
}

