#include "Fpch.h"
#include "D3D11Renderer.h"
#include "D3D11Window.h"
#include "D3D11Device.h"
#include "D3D11Context.h"
#include "D3D11Swapchain.h"
#include "D3D11RenderTargetView.h"
#include "D3D11VertexBuffer.h"

D3D11Renderer::D3D11Renderer( Uint32 width, Uint32 height )
{
	m_window = std::make_unique< D3D11Window >( GetModuleHandle( NULL ), width, height );
	FORGE_ASSERT( m_window->IsInitialized() );

	InitializeSwapChainAndContext( width, height, *m_window );

	m_renderTargetView = std::make_unique< D3D11RenderTargetView >( GetContext(), *m_device, *m_swapChain );

	m_renderTargetView->Set();

	InitializeViewport( width, height );
}

D3D11Renderer::~D3D11Renderer() = default;

D3D11VertexShader* D3D11Renderer::GetVertexShader( const std::string& path )
{
	m_vertexShaders.emplace_back( std::make_unique< D3D11VertexShader >( GetContext(), *m_device, path ) );
	return m_vertexShaders.back().get();
}

D3D11PixelShader* D3D11Renderer::GetPixelShader( const std::string& path )
{
	m_pixelShaders.emplace_back( std::make_unique< D3D11PixelShader >( GetContext(), *m_device, path ) );
	return m_pixelShaders.back().get();
}

std::unique_ptr< D3D11InputLayout > D3D11Renderer::GetInputLayout( const D3D11VertexShader& vertexShader, const D3D11VertexBuffer& vertexBuffer ) const
{
	return std::make_unique< D3D11InputLayout >( GetContext(), *m_device, vertexShader, vertexBuffer );
}

std::unique_ptr< D3D11VertexBuffer > D3D11Renderer::GetVertexBuffer( const Vertex* vertices, Uint32 amount ) const
{
	return std::make_unique< D3D11VertexBuffer >( GetContext(), *m_device, vertices, amount );
}

std::unique_ptr< D3D11IndexBuffer > D3D11Renderer::GetIndexBuffer( const Uint32* indices, Uint32 amount ) const
{
	return std::make_unique< D3D11IndexBuffer >( GetContext(), *m_device, indices, amount );
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

	m_device = std::make_unique< D3D11Device >( d3d11Device );
	m_context = std::make_unique< D3D11Context >( d3d11DevCon );
	m_swapChain = std::make_unique< D3D11Swapchain >( swapChain );
}

void D3D11Renderer::InitializeViewport( Uint32 width, Uint32 height )
{
	D3D11_VIEWPORT viewport;
	ZeroMemory( &viewport, sizeof( D3D11_VIEWPORT ) );

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast< Float >( width );
	viewport.Height = static_cast< Float >( height );

	//Set the Viewport
	m_context->GetDeviceContext()->RSSetViewports( 1, &viewport );
}
