#pragma once
#include "IRenderingResource.h"

namespace renderer
{
	class IVertexBuffer;
	class IIndexBuffer;

	class Model : public IRenderingResource
	{
	public:
		struct Shape
		{
			std::vector< Uint32 > m_indices;
			std::unique_ptr< renderer::IIndexBuffer > m_indexBuffer;
			Uint32 m_materialIndex = 0u;
		};

		Model( renderer::Renderer& renderer, renderer::Vertices vertices, const forge::ArraySpan< const renderer::Shape >& shapes );

		const renderer::IVertexBuffer* GetVertexBuffer() const
		{
			return m_vertexBuffer.get();
		}

		const renderer::Vertices& GetVertices() const
		{
			return m_vertices;
		}

		std::vector< Shape >& GetShapes()
		{
			return m_shapes;
		}

		const std::vector< Shape >& GetShapes() const
		{
			return m_shapes;
		}

	private:
		renderer::Vertices m_vertices;
		std::unique_ptr< renderer::IVertexBuffer > m_vertexBuffer;
		std::vector< Shape > m_shapes;
	};
}

