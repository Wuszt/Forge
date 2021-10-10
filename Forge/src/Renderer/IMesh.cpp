#include "Fpch.h"
#include "IMesh.h"
#include "../Renderer/IVertexBuffer.h"
#include "../Renderer/IIndexBuffer.h"
#include "../Renderer/IRenderer.h"

renderer::CubeMesh::CubeMesh( renderer::IRenderer& renderer )
{
	renderer::Vertices< renderer::IVertex< renderer::InputPosition > > vertices(
		{
			{ renderer::InputPosition( -1.0f, -1.0f, -1.0f ) },
			{ renderer::InputPosition( 1.0f,	-1.0f, -1.0f ) },
			{ renderer::InputPosition( 1.0f, 1.0f, -1.0f ) },
			{ renderer::InputPosition( -1.0f, 1.0f, -1.0f ) },

			{ renderer::InputPosition( -1.0f, -1.0f, 1.0f ) },
			{ renderer::InputPosition( 1.0f,	-1.0f, 1.0f ) },
			{ renderer::InputPosition( 1.0f, 1.0f, 1.0f ) },
			{ renderer::InputPosition( -1.0f, 1.0f, 1.0f ) },
		} );

	m_vertexBuffer = renderer.CreateVertexBuffer( vertices );

	Uint32 indices[ 6 * 6 ] =
	{
		0, 3, 1, 3, 2, 1,
		1, 2, 5, 2, 6, 5,
		5, 6, 4, 6, 7, 4,
		4, 7, 0, 7, 3, 0,
		3, 7, 2, 7, 6, 2,
		4, 0, 5, 0, 1, 5
	};

	m_indexBuffer = renderer.CreateIndexBuffer( indices, sizeof( indices ) / sizeof( Uint32 ) );
}

renderer::CubeMesh::~CubeMesh() = default;

const renderer::IVertexBuffer* renderer::CubeMesh::GetVertexBuffer() const
{
	return m_vertexBuffer.get();
}

const renderer::IIndexBuffer* renderer::CubeMesh::GetIndexBuffer() const
{
	return m_indexBuffer.get();
}
