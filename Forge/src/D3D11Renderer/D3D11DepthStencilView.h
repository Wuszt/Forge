#pragma once
#include "../Renderer/IDepthStencilView.h"

namespace d3d11
{
	class D3D11Texture;
	class D3D11DepthStencilView : public renderer::IDepthStencilView
	{
	public:
		D3D11DepthStencilView( const D3D11Device& device, const D3D11RenderContext& context, ID3D11Texture2D& texture, Uint32 textureIndex = 0u );
		~D3D11DepthStencilView();

		ID3D11DepthStencilView& GetView() const
		{
			return *m_view;
		}

		virtual void Clear() override;

	private:
		ID3D11DepthStencilView* m_view;
		const D3D11Device& m_device;
		const D3D11RenderContext& m_context;
	};
}

