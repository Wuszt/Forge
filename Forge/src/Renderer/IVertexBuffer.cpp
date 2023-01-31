#include "Fpch.h"
#include "IVertexBuffer.h"

renderer::Vertices renderer::VerticesBuilder::Build()
{
	return Vertices( *this );
}

renderer::VerticesBuilder::IElementDesc::IElementDesc( InputElementDescription desc )
	: m_description( std::move( desc ) )
{}

renderer::VerticesBuilder::IElementDesc::~IElementDesc() = default;
