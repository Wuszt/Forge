#include "Fpch.h"
#include "Model.h"
#include "../Renderer/IVertexBuffer.h"
#include "../Renderer/IIndexBuffer.h"
#include "../Renderer/IRenderer.h"
#include "IModelsLoader.h"

renderer::Model::Model( renderer::IRenderer& renderer, const renderer::Vertices& vertices, const std::vector< renderer::Shape >& shapes )
{
	m_vertexBuffer = renderer.CreateVertexBuffer( vertices );

	for( const auto& shape : shapes )
	{
		m_shapes.emplace_back( Model::Shape{ renderer.CreateIndexBuffer( shape.m_indices.data(), static_cast< Uint32 >( shape.m_indices.size() ) ), shape.m_materialIndex } );
	}
}
