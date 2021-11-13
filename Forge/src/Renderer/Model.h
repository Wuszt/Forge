#pragma once

namespace renderer
{
	class IVertexBuffer;
	class IIndexBuffer;

	class Model
	{
	public:
		struct Shape
		{
			std::unique_ptr< renderer::IIndexBuffer > m_indexBuffer;
			Uint32 m_materialIndex = 0u;
		};

		Model( renderer::IRenderer& renderer, const renderer::Vertices& vertices, const std::vector< renderer::Shape >& shapes );

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

