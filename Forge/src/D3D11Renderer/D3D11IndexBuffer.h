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
		D3D11IndexBuffer( D3D11RenderContext* contextPtr, const D3D11Device& device, const Uint32* indices, Uint32 amount );
		~D3D11IndexBuffer();

		FORGE_INLINE ID3D11Buffer* const& GetBuffer() const
		{
			return m_buffer;
		}

		void Set( Uint32 offset );

	private:
		ID3D11Buffer* m_buffer = nullptr;
		D3D11RenderContext* m_contextPtr = nullptr;
	};
}
