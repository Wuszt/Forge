#include "Fpch.h"
#include "ConstantBuffer.h"

renderer::IConstantBuffer::IConstantBuffer() = default;
renderer::IConstantBuffer::~IConstantBuffer() = default;
renderer::IConstantBuffer::IConstantBuffer( IConstantBuffer&& ) = default;

Bool renderer::ConstantBuffer::ContainsElement( const std::string& name ) const
{
	auto it = std::find_if( m_elements.begin(), m_elements.end(), [&]( const Element& el )
		{
			return el.m_name == name;
		});

	return it != m_elements.end();
}
