#pragma once
#include "../Renderer/IIndexBuffer.h"


struct ID3D11Buffer;

namespace d3d11
{
	class D3D11Device;
	class D3D11RenderContext;

	class D3D11IndexBuffer : public renderer::IIndexBuffer
	{
	public:
		D3D11IndexBuffer( D3D11RenderContext* contextPtr, const D3D11Device& device, forge::ArraySpan< const Uint32 > indices );
		~D3D11IndexBuffer();

		ID3D11Buffer* GetBuffer() const
		{
			return m_buffer;
		}

		virtual void Set( Uint32 offset ) const override;

		virtual Uint32 GetIndicesAmount() const override;

	private:
		ID3D11Buffer* m_buffer = nullptr;
		D3D11RenderContext* m_contextPtr = nullptr;
		Uint32 m_indicesAmount = 0u;
	};
}
