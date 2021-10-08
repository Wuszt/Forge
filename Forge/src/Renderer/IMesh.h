#pragma once

namespace renderer
{
	class IVertexBuffer;
	class IIndexBuffer;
	struct IVertices;

	class IMesh
	{
	public:
		virtual ~IMesh() = default;

		virtual const renderer::IVertexBuffer* GetVertexBuffer() const = 0;
		virtual const renderer::IIndexBuffer* GetIndexBuffer() const = 0;
	};

	class CubeMesh : public IMesh
	{
	public:
		CubeMesh( renderer::IRenderer& renderer );
		virtual ~CubeMesh();

		virtual const renderer::IVertexBuffer* GetVertexBuffer() const override;
		virtual const renderer::IIndexBuffer* GetIndexBuffer() const override;

	private:
		std::unique_ptr< renderer::IVertexBuffer > m_vertexBuffer;
		std::unique_ptr< renderer::IIndexBuffer > m_indexBuffer;
	};
}

