#include "Fpch.h"
#include "D3D11RenderTargetView.h"
#include "D3D11Device.h"
#include "D3D11SwapChain.h"
#include "D3D11Texture.h"

D3D11RenderTargetView::D3D11RenderTargetView( const D3D11Device& device, const D3D11Swapchain& swapChain )
{
	auto backBuffer = swapChain.GetBackBuffer();
	device.GetDevice()->CreateRenderTargetView( backBuffer->GetTexture(), NULL, &m_renderTargetView );
}

D3D11RenderTargetView::~D3D11RenderTargetView()
{
	m_renderTargetView->Release();
}
