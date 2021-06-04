#include "Fpch.h"
#include "D3D11RenderTargetView.h"
#include "D3D11Device.h"
#include "D3D11SwapChain.h"
#include "D3D11Texture.h"

D3D11RenderTargetView::D3D11RenderTargetView( D3D11RenderContext* contextPtr, const D3D11Device& device, const D3D11Swapchain& swapChain )
	: m_contextPtr( contextPtr )
{
	auto backBuffer = swapChain.GetBackBuffer();
	device.GetDevice()->CreateRenderTargetView( backBuffer->GetTexture(), NULL, &m_renderTargetView );
}

D3D11RenderTargetView::~D3D11RenderTargetView()
{
	m_renderTargetView->Release();
}

void D3D11RenderTargetView::Set()
{
	m_contextPtr->GetDeviceContext()->OMSetRenderTargets( 1, &m_renderTargetView, NULL );
}

void D3D11RenderTargetView::Clear( const Vector4& bgColor )
{
	m_contextPtr->GetDeviceContext()->ClearRenderTargetView( m_renderTargetView, bgColor.AsArray() );
}
