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
			Serialize( ::rtti::GetTypeInstanceOf< T >(), &data );
		}

		void Serialize( const rtti::Type& type, const void* address );

	private:
		Serializer() = default;

		void SerializePrimitive( const rtti::Type& type, const void* address );
		void SerializeString( const rtti::StringType& type, const void* address );
		void SerializeClassOrStruct( const rtti::Type& type, const void* address );
		void SerializeArray( const rtti::ContainerType& type, const void* address );
		void SerializeDynamicContainer( const rtti::ContainerType& type, const void* address );
		void SerializeUniquePointer( const rtti::UniquePtrBaseType& type, const void* address );
		void SerializeSharedPointer( const rtti::SharedPtrBaseType& type, const void* address );

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
			Deserialize( ::rtti::GetTypeInstanceOf< T >(), &data );
		}

		template< class T >
		T Deserialize()
		{
			T result;
			Deserialize( result );
			return result;
		}

		void Deserialize( const rtti::Type& type, void* address );

	private:
		Deserializer() = default;

		void DeserializePrimitive( const rtti::Type& type, void* address );
		void DeserializeString( const rtti::StringType& type, void* address );
		void DeserializeClassOrStruct( const rtti::Type& type, void* address );
		void DeserializeArray( const rtti::ContainerType& type, void* address );
		void DeserializeDynamicContainer( const rtti::ContainerType& type, void* address );
		void DeserializeUniquePointer( const rtti::UniquePtrBaseType& type, void* address );
		void DeserializeSharedPointer( const rtti::SharedPtrBaseType& type, void* address );

		std::unordered_map< Uint64, void* > m_sharedPtrsAddresses;
		Stream* m_stream = nullptr;
	};
}

