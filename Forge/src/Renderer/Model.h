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
			std::unique_ptr< renderer::IIndexBuffer > m_indexBuffer;
			Uint32 m_materialIndex = 0u;
		};

		Model( renderer::Renderer& renderer, const renderer::Vertices& vertices, const forge::ArraySpan< const renderer::Shape >& shapes );

		const renderer::IVertexBuffer* GetVertexBuffer() const
		{
			return m_vertexBuffer.get();
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
		std::unique_ptr< renderer::IVertexBuffer > m_vertexBuffer;
		std::vector< Shape > m_shapes;
	};
}

