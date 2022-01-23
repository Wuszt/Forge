#include "Fpch.h"
#include "D3D11RenderTargetView.h"
#include "D3D11Device.h"
#include "D3D11Texture.h"

namespace d3d11
{
	D3D11RenderTargetView::D3D11RenderTargetView( const D3D11Device& device, const D3D11RenderContext& context, std::shared_ptr< D3D11Texture > texture )
		: m_context( context )
		, m_texture( texture )
	{
		FORGE_ASSERT( texture );

		device.GetDevice()->CreateRenderTargetView( texture->GetTexture(), NULL, &m_renderTargetView );
	}

	D3D11RenderTargetView::~D3D11RenderTargetView()
	{
		m_renderTargetView->Release();
	}

	std::shared_ptr< renderer::ITexture > D3D11RenderTargetView::GetTexture() const
	{
		return std::static_pointer_cast<renderer::ITexture>( m_texture );
	}

	void D3D11RenderTargetView::Clear( const Vector4& bgColor )
	{
		m_context.GetDeviceContext()->ClearRenderTargetView( m_renderTargetView, bgColor.AsArray() );
	}
}