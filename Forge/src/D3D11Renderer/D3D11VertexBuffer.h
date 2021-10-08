#pragma once
#include "../Renderer/IVertexBuffer.h"

struct D3D11_INPUT_ELEMENT_DESC;
struct ID3D11Buffer;

namespace d3d11
{
	class D3D11RenderContext;
	class D3D11Device;

	class D3D11VertexBuffer : public renderer::IVertexBuffer
	{
	public:
		D3D11VertexBuffer( D3D11RenderContext* contextPtr, const D3D11Device& device, const renderer::IVertices& vertices );
		~D3D11VertexBuffer();

		ID3D11Buffer* const& GetBuffer() const
		{
			return m_buffer;
		}

		Uint32 GetStride() const
		{
			return m_stride;
		}

		virtual void Set() const override;

		const D3D11_INPUT_ELEMENT_DESC* GetLayout() const;

		Uint32 GetElementsAmount() const;

	private:
		ID3D11Buffer* m_buffer;
		D3D11RenderContext* m_contextPtr;
		Uint32 m_stride = 0u;
		std::vector< D3D11_INPUT_ELEMENT_DESC > m_layout;
	};
}
