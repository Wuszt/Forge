#include "Fpch.h"
#include "D3D11Renderer.h"
#include "../Core/WindowsWindow.h"
#include "D3D11ConstantBufferImpl.h"

namespace d3d11
{
	D3D11Renderer::D3D11Renderer( forge::IWindow& window )
	{
		FORGE_ASSERT( dynamic_cast<windows::WindowsWindow*>( &window ) );
		InitializeSwapChainAndContext( static_cast<windows::WindowsWindow&>( window ) );

		m_renderTargetView = std::make_unique< D3D11RenderTargetView >( *GetDevice(), *GetContext(), std::move( m_swapChain->GetBackBuffer() ) );
		m_depthStencilBuffer = std::make_unique< D3D11DepthStencilBuffer >( *GetDevice(), *GetContext(), window.GetWidth(), window.GetHeight() );

		std::vector< renderer::IRenderTargetView* > views{ GetRenderTargetView() };

		SetRenderTargets( views, m_depthStencilBuffer.get() );

		InitializeViewport( window.GetWidth(), window.GetHeight() );

		InitializeRasterizer();

		m_windowCallbackToken = window.RegisterEventListener(
			[ & ]( const forge::IWindow::IEvent& event )
		{
			switch( event.GetEventType() )
			{
			case forge::IWindow::EventType::OnWindowResized:
				FORGE_ASSERT( dynamic_cast<const forge::IWindow::OnResizedWindowEvent*>( &event ) );
				const forge::IWindow::OnResizedWindowEvent& resizedEvent = static_cast<const forge::IWindow::OnResizedWindowEvent&>( event );

				m_renderTargetView = nullptr;
				GetSwapchain()->Resize( resizedEvent.GetWidth(), resizedEvent.GetHeight() );

				m_renderTargetView = std::make_unique< D3D11RenderTargetView >( *GetDevice(), *GetContext(), std::move( GetSwapchain()->GetBackBuffer() ) );
				m_depthStencilBuffer = std::make_unique< D3D11DepthStencilBuffer >( *GetDevice(), *GetContext(), resizedEvent.GetWidth(), resizedEvent.GetHeight() );

				std::vector< renderer::IRenderTargetView* > views{ GetRenderTargetView() };
				SetRenderTargets( views, m_depthStencilBuffer.get() );

				InitializeViewport( resizedEvent.GetWidth(), resizedEvent.GetHeight() );
				break;
			}
		} );
	}

	D3D11Renderer::~D3D11Renderer()
	{
		m_rasterizerState->Release();
	}

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

	std::unique_ptr< renderer::IInputLayout > D3D11Renderer::CreateInputLayout( const renderer::IVertexShader& vertexShader, const renderer::IVertexBuffer& vertexBuffer ) const
	{
		FORGE_ASSERT( dynamic_cast<const D3D11VertexShader*>( &vertexShader ) );
		return std::make_unique< D3D11InputLayout >( GetContext(), *m_device, static_cast<const D3D11VertexShader&>( vertexShader ), static_cast<const D3D11VertexBuffer&>( vertexBuffer ) );
	}

	std::unique_ptr< renderer::IVertexBuffer > D3D11Renderer::CreateVertexBuffer( const renderer::IVertices& vertices ) const
	{
		return std::make_unique< D3D11VertexBuffer >( GetContext(), *m_device, vertices );
	}

	std::unique_ptr< renderer::IIndexBuffer > D3D11Renderer::CreateIndexBuffer( const Uint32* indices, Uint32 amount ) const
	{
		return std::make_unique< D3D11IndexBuffer >( GetContext(), *m_device, indices, amount );
	}

	void D3D11Renderer::SetRenderTargets( std::vector< renderer::IRenderTargetView* > rendererTargetViews, renderer::IDepthStencilBuffer* depthStencilBuffer )
	{
		FORGE_ASSERT( dynamic_cast<D3D11DepthStencilBuffer*>( depthStencilBuffer ) );
		SetRenderTargets( rendererTargetViews, static_cast<D3D11DepthStencilBuffer*>( depthStencilBuffer ) );
	}

	void D3D11Renderer::SetRenderTargets( std::vector< renderer::IRenderTargetView* > rendererTargetViews, D3D11DepthStencilBuffer* depthStencilBuffer )
	{
		std::vector< ID3D11RenderTargetView* > views;
		views.reserve( rendererTargetViews.size() );

		for( const auto& target : rendererTargetViews )
		{
			FORGE_ASSERT( dynamic_cast<D3D11RenderTargetView*>( target ) );
			views.emplace_back( static_cast<D3D11RenderTargetView*>( target )->GetRenderTargetView() );
		}

		m_context->GetDeviceContext()->OMSetRenderTargets( static_cast<Uint32>( views.size() ), views.data(), m_depthStencilBuffer ? m_depthStencilBuffer->GetView() : nullptr );
	}

	void D3D11Renderer::BeginScene()
	{
		{
			struct cbPerFrame
			{
				Float time;
				Float padding[ 3 ];
			};

			auto buff = GetConstantBuffer< cbPerFrame >();

			buff->GetData().time = forge::Time::GetTime();
			buff->SetVS( 0 );
		}
	}

	std::unique_ptr< renderer::IConstantBufferImpl > D3D11Renderer::CreateConstantBufferImpl( void* data, Uint32 dataSize ) const
	{
		return std::make_unique< D3D11ConstantBufferImpl >( m_context.get(), m_device.get(), data, dataSize );
	}

	void D3D11Renderer::InitializeSwapChainAndContext( const windows::WindowsWindow& window )
	{
		auto swapChainDesc = D3D11Swapchain::GetSwapChainDescription( window.GetWidth(), window.GetHeight(), window.GetHWND() );

		IDXGISwapChain* swapChain;
		ID3D11Device* d3d11Device;
		ID3D11DeviceContext* d3d11DevCon;

		FORGE_ASSURE( D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
			D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &d3d11Device, NULL, &d3d11DevCon ) == S_OK );

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
		viewport.Width = static_cast<Float>( width );
		viewport.Height = static_cast<Float>( height );
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		m_context->GetDeviceContext()->RSSetViewports( 1, &viewport );
	}

	void D3D11Renderer::InitializeRasterizer()
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory( &rasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );
		rasterizerDesc.AntialiasedLineEnable = false;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;;
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;

		m_device->GetDevice()->CreateRasterizerState( &rasterizerDesc, &m_rasterizerState );
		m_context->GetDeviceContext()->RSSetState( m_rasterizerState );
	}
}