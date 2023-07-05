#pragma once

namespace forge
{
	class Serializer
	{
	public:
		enum class Mode
		{
			Saving,
			Loading,
		};

		Serializer( Mode mode )
			: m_mode( mode )
		{}

		Serializer( Serializer& ) = delete;
		Serializer( Serializer&& ) = delete;

		Serializer( Mode mode, Serializer&& other )
			: m_stream( std::move( other.m_stream ) )
			, m_mode( mode )
		{
			m_stream.clear();
			m_stream.seekp( 0u, std::ios_base::beg );
		}

		template< class T >
		void Serialize( T& data )
		{
			SerializeType( ::rtti::GetTypeInstanceOf< T >(), &data );
		}

	private:
		void SerializeType( const rtti::Type& type, void* address );
		void SerializePrimitive( const rtti::Type& type, void* address );
		void SerializeClassOrStruct( const rtti::Type& type, void* address );
		void SerializeArray( const rtti::ContainerType& type, void* address );
		void SerializeDynamicContainer( const rtti::ContainerType& type, void* address );
		void SerializeUniquePointer( const rtti::UniquePtrBaseType& type, void* address );

		std::stringstream m_stream;
		Mode m_mode;
	};
}

