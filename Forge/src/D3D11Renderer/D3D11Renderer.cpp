#include "Fpch.h"
#include "D3D11Renderer.h"
#include "../Core/WindowsWindow.h"
#include "D3D11ConstantBufferImpl.h"

namespace d3d11
{
	struct RawRenderable
	{
		Uint32 m_VBStride : 16; // 2
		Uint32 m_IBOffset : 16; // 4
		Uint32 m_indicesAmount; //8
		ID3D11Buffer* m_vertexBuffer; // 16

		ID3D11Buffer* m_indexBuffer; // 24;
		ID3D11VertexShader* m_vertexShader; // 32

		ID3D11PixelShader* m_pixelShader; // 40
		ID3D11InputLayout* m_inputLayout; // 48

		ID3D11Buffer* m_meshCB; // 56
		ID3D11Buffer* m_materialCB; // 64;
	};

	static_assert( sizeof( RawRenderable ) == 64, "RawRenderable is larger than before!" );

	D3D11Renderer::D3D11Renderer( forge::IWindow& window )
	{
		FORGE_ASSERT( dynamic_cast<windows::WindowsWindow*>( &window ) );
		InitializeSwapChainAndContext( static_cast<windows::WindowsWindow&>( window ) );

		m_renderTargetView = std::make_unique< D3D11RenderTargetView >( *GetDevice(), *GetContext(), std::move( m_swapChain->GetBackBuffer() ) );
		m_depthStencilBuffer = std::make_unique< D3D11DepthStencilBuffer >( *GetDevice(), *GetContext(), window.GetWidth(), window.GetHeight() );
		m_shadersManager = std::make_unique< D3D11ShadersManager >( *GetDevice(), *GetContext() );

		std::vector< renderer::IRenderTargetView* > views{ GetRenderTargetView() };

		SetRenderTargets( views, m_depthStencilBuffer.get() );

		InitializeViewport( window.GetWidth(), window.GetHeight() );

		InitializeRasterizer();

		m_windowCallbackToken = std::make_unique< forge::CallbackToken >( window.RegisterEventListener(
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
		} ) );
	}

	D3D11Renderer::~D3D11Renderer()
	{
		m_rasterizerState->Release();
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
			struct cbFrame
			{
				Float time;
				Float padding[ 3 ];
			};

			auto buff = CreateStaticConstantBuffer< cbFrame >();

			buff->GetData().time = forge::Time::GetTime();
			buff->SetVS( renderer::VSConstantBufferType::Frame );
		}
	}

	std::unique_ptr< forge::IDataPackage > D3D11Renderer::CreateRawRenderablesPackage( const std::vector< const renderer::Renderable* >& renderables ) const
	{
		auto result = std::make_unique< forge::DataPackage< RawRenderable > >();

		for( auto& renderable : renderables )
		{
			const D3D11VertexBuffer* vb = static_cast< const D3D11VertexBuffer* >( renderable->GetMesh()->GetVertexBuffer() );
			const D3D11IndexBuffer* ib = static_cast< const D3D11IndexBuffer* >( renderable->GetMesh()->GetIndexBuffer() );
			const D3D11VertexShader* vs = static_cast< const D3D11VertexShader* >( renderable->GetMaterial()->GetVertexShader() );
			const D3D11PixelShader* ps = static_cast< const D3D11PixelShader* >( renderable->GetMaterial()->GetPixelShader() );
			const D3D11InputLayout* il = static_cast< const D3D11InputLayout* >( renderable->GetInputLayout() );
			const D3D11ConstantBufferImpl* meshCBImpl = static_cast< const D3D11ConstantBufferImpl* >( renderable->GetCBMesh().GetImpl() );
			const D3D11ConstantBufferImpl* materialCBImpl = static_cast<const D3D11ConstantBufferImpl*>( renderable->GetMaterial()->GetConstantBuffer()->GetImpl() );
			result->AddData( RawRenderable{ vb->GetStride(), 0u, ib->GetIndicesAmount(), vb->GetBuffer(), ib->GetBuffer(), vs->GetShader(), ps->GetShader(), il->GetLayout(), meshCBImpl->GetBuffer(), materialCBImpl->GetBuffer() } );
		}

		return result;
	}

	void D3D11Renderer::Draw( const forge::IDataPackage& rawRenderables )
	{
		auto* context = GetContext()->GetDeviceContext();

		const forge::DataPackage < RawRenderable >& renderables = static_cast< const forge::DataPackage < RawRenderable >& >( rawRenderables );

		Uint32 dataSize = renderables.GetDataSize();

		for( Uint32 i = 0; i < dataSize; ++i )
		{
			Uint32 offset = 0u;
			Uint32 stride = renderables[ i ].m_VBStride;

			context->IASetVertexBuffers( 0, 1, &renderables[ i ].m_vertexBuffer, &stride, &offset );
			context->IASetIndexBuffer( renderables[ i ].m_indexBuffer, DXGI_FORMAT_R32_UINT, renderables[ i ].m_IBOffset );
			context->VSSetShader( renderables[ i ].m_vertexShader, 0, 0 );
			context->PSSetShader( renderables[ i ].m_pixelShader, 0, 0 );
			context->IASetInputLayout( renderables[ i ].m_inputLayout );
			context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			context->VSSetConstantBuffers( static_cast< Uint32 >( renderer::VSConstantBufferType::Mesh ), 1, &renderables[ i ].m_meshCB );
			context->VSSetConstantBuffers( static_cast<Uint32>( renderer::VSConstantBufferType::Material ), 1, &renderables[ i ].m_materialCB );

			GetContext()->Draw( renderables[ i ].m_indicesAmount, 0 );
		}
	}

	std::unique_ptr< renderer::IConstantBufferImpl > D3D11Renderer::CreateConstantBufferImpl() const
	{
		return std::make_unique< D3D11ConstantBufferImpl >( *m_context, *m_device );
	}

	void D3D11Renderer::InitializeSwapChainAndContext( const windows::WindowsWindow& window )
	{
		auto swapChainDesc = D3D11Swapchain::GetSwapChainDescription( window.GetWidth(), window.GetHeight(), window.GetHWND() );

		IDXGISwapChain* swapChain;
		ID3D11Device* d3d11Device;
		ID3D11DeviceContext* d3d11DevCon;

		Uint32 flags = 0u;
		
#ifdef FORGE_DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		FORGE_ASSURE( D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, NULL, NULL,
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