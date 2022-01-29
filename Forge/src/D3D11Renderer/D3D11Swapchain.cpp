#include "Fpch.h"
#include "D3D11Swapchain.h"
#include "D3D11Texture.h"

namespace d3d11
{
	const DXGI_FORMAT c_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	DXGI_SWAP_CHAIN_DESC D3D11Swapchain::GetSwapChainDescription( Uint32 width, Uint32 height, HWND hwnd )
	{
		DXGI_MODE_DESC bufferDesc;

		ZeroMemory( &bufferDesc, sizeof( DXGI_MODE_DESC ) );

		bufferDesc.Width = width;
		bufferDesc.Height = height;
		bufferDesc.RefreshRate.Numerator = 60;
		bufferDesc.RefreshRate.Denominator = 1;
		bufferDesc.Format = c_backBufferFormat;
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

	class SwapchainTexture : public D3D11Texture
	{
	public:
		using D3D11Texture::D3D11Texture;

		void OnBeforeSwapchainResize()
		{
			ReleaseResources();
		}

		void OnSwapchainResized( ID3D11Texture2D& texture )
		{
			SetTexture( texture );

			CreateViewsIfRequired( D3D11_BIND_RENDER_TARGET, c_backBufferFormat );
			InvokeResizeCallback( GetTextureSize() );
		}
	};

	D3D11Swapchain::D3D11Swapchain( const D3D11Device& device, const D3D11RenderContext& context, IDXGISwapChain* swapChain )
		: m_swapChain( swapChain )
		, m_device( device )
		, m_context( context )
	{
		ID3D11Texture2D* backBuffer;
		FORGE_ASSURE( m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&backBuffer ) == S_OK );

		m_backBuffer = std::make_unique< SwapchainTexture >( m_device, m_context, *backBuffer );
	}

	D3D11Swapchain::~D3D11Swapchain()
	{
		m_swapChain->Release();
	}

	void D3D11Swapchain::Present()
	{
		m_swapChain->Present( 0, 0 );
	}

	renderer::ITexture& D3D11Swapchain::GetBackBuffer() const
	{
		return *m_backBuffer;
	}

	void D3D11Swapchain::Resize( Uint32 width, Uint32 height )
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		m_swapChain->GetDesc( &swapChainDesc );
		m_backBuffer->OnBeforeSwapchainResize();
		FORGE_ASSURE( m_swapChain->ResizeBuffers( swapChainDesc.BufferCount, width, height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags ) == S_OK );

		ID3D11Texture2D* backBuffer;
		FORGE_ASSURE( m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&backBuffer ) == S_OK );
		m_backBuffer->OnSwapchainResized( *backBuffer );
	}
}