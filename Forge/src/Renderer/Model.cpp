#include "Fpch.h"
#include "Model.h"
#include "../Renderer/IVertexBuffer.h"
#include "../Renderer/IIndexBuffer.h"
#include "../Renderer/Renderer.h"

renderer::Model::Model( renderer::Renderer& renderer, renderer::Vertices vertices, const forge::ArraySpan< const renderer::Shape >& shapes )
{
	m_vertexBuffer = renderer.CreateVertexBuffer( vertices );
	m_vertices = std::move( vertices );

	for ( const auto& shape : shapes )
	{
		m_shapes.emplace_back( Model::Shape{ shape.m_indices, renderer.CreateIndexBuffer( shape.m_indices.data(), static_cast< Uint32 >( shape.m_indices.size() ) ), shape.m_materialIndex } );
	}
}
