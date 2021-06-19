#include "Fpch.h"
#include "D3D11Renderer.h"
#include "D3D11Device.h"
#include "D3D11RenderContext.h"
#include "D3D11Swapchain.h"
#include "D3D11RenderTargetView.h"
#include "D3D11VertexBuffer.h"
#include "../Core/WindowsWindow.h"
#include "../Renderer/ICamera.h"

D3D11Renderer::D3D11Renderer( const IWindow& window )
{
	FORGE_ASSERT( dynamic_cast< const WindowsWindow* >( &window ) );
	InitializeSwapChainAndContext( static_cast< const WindowsWindow& >( window ) );

	m_renderTargetView = std::make_unique< D3D11RenderTargetView >( GetContext(), *m_device, *m_swapChain );

	m_renderTargetView->Set();

	InitializeViewport( window.GetWidth(), window.GetHeight() );
}

D3D11Renderer::~D3D11Renderer() = default;

D3D11VertexShader* D3D11Renderer::GetVertexShader( const std::string& path )
{
	m_vertexShaders.emplace_back( std::make_unique< D3D11VertexShader >( GetContext(), *m_device, "Shaders\\" + path ) );
	return m_vertexShaders.back().get();
}

D3D11PixelShader* D3D11Renderer::GetPixelShader( const std::string& path )
{
	m_pixelShaders.emplace_back( std::make_unique< D3D11PixelShader >( GetContext(), *m_device, "Shaders\\" + path ) );
	return m_pixelShaders.back().get();
}

std::unique_ptr< IInputLayout > D3D11Renderer::GetInputLayout( const IVertexShader& vertexShader, const IVertexBuffer& vertexBuffer ) const
{
	FORGE_ASSERT( dynamic_cast< const D3D11VertexShader* >( &vertexShader ) );
	return std::make_unique< D3D11InputLayout >( GetContext(), *m_device, static_cast< const D3D11VertexShader& >( vertexShader ), static_cast<const D3D11VertexBuffer&>( vertexBuffer )  );
}

std::unique_ptr< IVertexBuffer > D3D11Renderer::GetVertexBuffer( const IVertices& vertices ) const
{
	return std::make_unique< D3D11VertexBuffer >( GetContext(), *m_device, vertices );
}

std::unique_ptr< IIndexBuffer > D3D11Renderer::GetIndexBuffer( const Uint32* indices, Uint32 amount ) const
{
	return std::make_unique< D3D11IndexBuffer >( GetContext(), *m_device, indices, amount );
}

struct cbPerObject
{
	Matrix WVP;
};

ID3D11Buffer* m_buff;
cbPerObject m_cb;

void D3D11Renderer::BeginScene( const ICamera& camera )
{
	static Bool initialized = false;

	if( !initialized )
	{
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory( &cbbd, sizeof( D3D11_BUFFER_DESC ) );

		cbbd.Usage = D3D11_USAGE_DEFAULT;
		cbbd.ByteWidth = sizeof( cbPerObject );
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		m_device->GetDevice()->CreateBuffer( &cbbd, nullptr, &m_buff );

		initialized = true;
	}

	m_cb.WVP = camera.GetViewProjectionMatrix();

	GetContext()->GetDeviceContext()->UpdateSubresource( m_buff, 0, nullptr, &m_cb, 0, 0 );
	GetContext()->GetDeviceContext()->VSSetConstantBuffers( 0, 1, &m_buff );
}

void D3D11Renderer::InitializeSwapChainAndContext( const WindowsWindow& window )
{
	auto swapChainDesc = D3D11Swapchain::GetSwapChainDescription( window.GetWidth(), window.GetHeight(), window.GetHWND() );

	IDXGISwapChain* swapChain;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;

	auto result = D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &d3d11Device, NULL, &d3d11DevCon );

	FORGE_ASSERT( result == S_OK );

	m_device = std::make_unique< D3D11Device >( d3d11Device );
	m_context = std::make_unique< D3D11RenderContext >( d3d11DevCon );
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

	m_context->GetDeviceContext()->RSSetViewports( 1, &viewport );
}
