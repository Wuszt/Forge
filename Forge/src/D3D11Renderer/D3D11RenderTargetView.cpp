#include "Fpch.h"
#include "D3D11RenderTargetView.h"
#include "D3D11Device.h"

D3D11RenderTargetView::D3D11RenderTargetView( D3D11RenderContext* contextPtr, const D3D11Device& device, std::shared_ptr< D3D11Texture > texture )
	: m_contextPtr( contextPtr )
	, m_texture( texture )
{
	FORGE_ASSERT( texture );

	device.GetDevice()->CreateRenderTargetView( texture->GetTexture(), NULL, &m_renderTargetView );
}

D3D11RenderTargetView::~D3D11RenderTargetView()
{
	m_renderTargetView->Release();
}

void D3D11RenderTargetView::Clear( const Vector4& bgColor )
{
	m_contextPtr->GetDeviceContext()->ClearRenderTargetView( m_renderTargetView, bgColor.AsArray() );
}
