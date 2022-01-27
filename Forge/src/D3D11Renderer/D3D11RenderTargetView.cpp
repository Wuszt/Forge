#include "Fpch.h"
#include "D3D11RenderTargetView.h"
#include "D3D11Device.h"
#include "D3D11Texture.h"

namespace d3d11
{
	D3D11RenderTargetView::D3D11RenderTargetView( const D3D11Device& device, const D3D11RenderContext& context, ID3D11Texture2D& texture )
		: m_context( context )
	{
		device.GetDevice()->CreateRenderTargetView( &texture, NULL, &m_renderTargetView );
	}

	D3D11RenderTargetView::~D3D11RenderTargetView()
	{
		m_renderTargetView->Release();
	}

	void D3D11RenderTargetView::Clear( const Vector4& bgColor )
	{
		m_context.GetDeviceContext()->ClearRenderTargetView( m_renderTargetView, bgColor.AsArray() );
	}
}