#include "Fpch.h"
#include "ConstantBuffer.h"
#include "../Core/Serializer.h"

RTTI_IMPLEMENT_TYPE( renderer::ConstantBuffer,
	RTTI_REGISTER_METHOD( Serialize );
	RTTI_REGISTER_METHOD( Deserialize );
);

RTTI_IMPLEMENT_TYPE( renderer::ConstantBuffer::Element,
	RTTI_REGISTER_METHOD( Serialize );
	RTTI_REGISTER_METHOD( Deserialize );
);

renderer::IConstantBuffer::IConstantBuffer() = default;
renderer::IConstantBuffer::~IConstantBuffer() = default;
renderer::IConstantBuffer::IConstantBuffer( IConstantBuffer&& ) = default;

renderer::ConstantBuffer::ConstantBuffer( const ConstantBuffer& toCopy )
{
	*this = toCopy;
}

renderer::ConstantBuffer& renderer::ConstantBuffer::operator=( const ConstantBuffer& toCopy )
{
	m_elements = std::vector< Element >( toCopy.m_elements );
	m_rawData = forge::UniqueRawPtr( toCopy.m_rawData.GetSize() );
	memcpy( m_rawData.GetData(), toCopy.m_rawData.GetData(), m_rawData.GetSize() );

	CreateBuffer();
	UpdateBuffer();
	return *this;
}

void renderer::ConstantBuffer::AddData( std::string name, const rtti::Type& type, const void* data )
{
	const Uint32 offset = static_cast< Uint32 >( type.GetSize() );
	const Uint64 prevSize = m_rawData.GetSize() - m_currentPadding;

	FORGE_ASSURE( !ContainsElement( name.c_str() ) );
	m_elements.push_back( { std::move( name ), &type } );

	forge::UniqueRawPtr prevData = std::move( m_rawData );

	const Uint64 newSize = prevData.GetSize() + offset;
	m_currentPadding = 16 - ( newSize % 16 );
	m_currentPadding = m_currentPadding == 16 ? 0 : m_currentPadding;
	m_rawData = std::move( forge::UniqueRawPtr( prevSize + offset + m_currentPadding ) );
	memcpy( m_rawData.GetData(), prevData.GetData(), prevSize );
	memcpy( static_cast< Byte* >( m_rawData.GetData() ) + prevSize, data, offset );

	CreateBuffer();
}

Bool renderer::ConstantBuffer::SetData( std::string name, const rtti::Type& type, const void* data )
{
	Uint32 offset = 0u;
	for ( const Element& element : m_elements )
	{
		if ( element.m_name == name )
		{
			if ( *element.m_type != type )
			{
				return false;
			}

			memcpy( static_cast< Byte* >( m_rawData.GetData() ) + offset, data, type.GetSize() );
			return true;
		}

		offset += static_cast< Uint32 >( element.m_type->GetSize() );
	}

	return false;
}

void renderer::ConstantBuffer::AddOrSetData( std::string name, const rtti::Type& type, const void* data )
{
	if ( ContainsElement( name.c_str() ) )
	{
		FORGE_ASSURE( SetData( std::move( name ), type, data ) );
	}
	else
	{
		AddData( std::move( name ), type, data );
	}
}

Bool renderer::ConstantBuffer::ContainsElement( const Char* name ) const
{
	auto it = std::find_if( m_elements.begin(), m_elements.end(), [&]( const Element& el )
		{
			return strcmp( el.m_name.c_str(), name ) == 0;
		});

	return it != m_elements.end();
}

void renderer::ConstantBuffer::OnPropertyChanged( const forge::PropertiesChain& propertiesChain )
{
	UpdateBuffer();
}

void renderer::ConstantBuffer::Serialize( forge::Serializer& serializer ) const
{
	serializer << m_elements;
	serializer << m_rawData;
}

void renderer::ConstantBuffer::Deserialize( forge::Deserializer& deserializer )
{
	deserializer >> m_elements;
	deserializer >> m_rawData;
}

void renderer::ConstantBuffer::Element::Serialize( forge::Serializer& serializer ) const
{
	serializer << m_name;
	serializer << m_type->GetID();
}

void renderer::ConstantBuffer::Element::Deserialize( forge::Deserializer& deserializer )
{
	deserializer >> m_name;

	rtti::ID id;
	deserializer >> id;
	m_type = rtti::Get().FindType( id );
}
