#pragma once

namespace forge
{
	class Stream;

	class Serializer
	{
	public:
		enum class Mode
		{
			Saving,
			Loading,
		};

		Serializer( Mode mode, Stream& stream );

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

		Stream& m_stream;
		Mode m_mode;
	};
}

