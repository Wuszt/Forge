#pragma once
#include "../Renderer/IDepthStencilBuffer.h"

struct ID3D11DepthStencilView;

namespace d3d11
{
	class D3D11DepthStencilBuffer : public renderer::IDepthStencilBuffer
	{
	public:
		D3D11DepthStencilBuffer( const D3D11Device& device, const D3D11RenderContext& context, Uint32 width, Uint32 height );
		~D3D11DepthStencilBuffer();

		FORGE_INLINE ID3D11DepthStencilView* GetView() const
		{
			return m_view;
		}

		virtual void Clear() override;

	private:
		std::shared_ptr< D3D11Texture > m_texture;
		ID3D11DepthStencilView* m_view;
		const D3D11Device& m_device;
		const D3D11RenderContext& m_context;
	};
}
