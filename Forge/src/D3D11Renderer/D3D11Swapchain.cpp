#include "Fpch.h"
#include "D3D11Swapchain.h"
#include "D3D11Texture.h"

namespace d3d11
{
	DXGI_SWAP_CHAIN_DESC D3D11Swapchain::GetSwapChainDescription( Uint32 width, Uint32 height, HWND hwnd )
	{
		DXGI_MODE_DESC bufferDesc;

		ZeroMemory( &bufferDesc, sizeof( DXGI_MODE_DESC ) );

		bufferDesc.Width = width;
		bufferDesc.Height = height;
		bufferDesc.RefreshRate.Numerator = 60;
		bufferDesc.RefreshRate.Denominator = 1;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		//Describe our SwapChain
		DXGI_SWAP_CHAIN_DESC swapChainDesc;

		ZeroMemory( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );

		swapChainDesc.BufferDesc = bufferDesc;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		return swapChainDesc;
	}

	D3D11Swapchain::D3D11Swapchain( IDXGISwapChain* swapChain )
		: m_swapChain( swapChain )
	{}

	D3D11Swapchain::~D3D11Swapchain()
	{
		m_swapChain->Release();
	}

	void D3D11Swapchain::Present()
	{
		m_swapChain->Present( 0, 0 );
	}

	std::unique_ptr< D3D11Texture > D3D11Swapchain::GetBackBuffer() const
	{
		ID3D11Texture2D* backBuffer;
		FORGE_ASSURE( m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&backBuffer ) == S_OK );

		return std::make_unique< D3D11Texture >( backBuffer );
	}

	void D3D11Swapchain::Resize( Uint32 width, Uint32 height )
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		m_swapChain->GetDesc( &swapChainDesc );
		FORGE_ASSURE( m_swapChain->ResizeBuffers( swapChainDesc.BufferCount, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags ) == S_OK );
	}
}