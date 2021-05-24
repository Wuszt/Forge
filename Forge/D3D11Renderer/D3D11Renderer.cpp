#include "Fpch.h"
#include "D3D11Renderer.h"
#include "D3D11Window.h"
#include "D3D11Device.h"
#include "D3D11Context.h"
#include "D3D11Swapchain.h"
#include "D3D11RenderTargetView.h"

D3D11Renderer::D3D11Renderer( Uint32 width, Uint32 height )
{
	m_window = std::make_unique< D3D11Window >( GetModuleHandle( NULL ), width, height );
	FORGE_ASSERT( m_window->IsInitialized() );

	InitializeSwapChainAndContext( width, height, *m_window );

	m_renderTargetView = std::make_unique< D3D11RenderTargetView >( *m_device, *m_swapChain );

	m_context->SetRenderTarget( *m_renderTargetView );
}

D3D11Renderer::~D3D11Renderer() = default;

D3D11VertexShader* D3D11Renderer::GetVertexShader( const std::string& path )
{
	m_vertexShaders.emplace_back( std::make_unique< D3D11VertexShader >( *m_device, path ) );
	return m_vertexShaders.back().get();
}

D3D11PixelShader* D3D11Renderer::GetPixelShader( const std::string& path )
{
	m_pixelShaders.emplace_back( std::make_unique< D3D11PixelShader >( *m_device, path ) );
	return m_pixelShaders.back().get();
}

void D3D11Renderer::InitializeSwapChainAndContext( Uint32 width, Uint32 height, const D3D11Window& window )
{
	auto swapChainDesc = D3D11Swapchain::GetSwapChainDescription( width, height, window.GetHWND() );

	IDXGISwapChain* swapChain;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;

	auto result = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &d3d11Device, NULL, &d3d11DevCon );

	FORGE_ASSERT( result == S_OK );

	m_device = std::make_unique< D3D11Device >(  d3d11Device );
	m_context = std::make_unique< D3D11Context >( d3d11DevCon );
	m_swapChain = std::make_unique< D3D11Swapchain >( swapChain );
}
