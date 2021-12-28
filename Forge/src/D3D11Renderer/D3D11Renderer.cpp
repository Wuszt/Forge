#include "Fpch.h"
#include "D3D11Renderer.h"
#include "../Core/WindowsWindow.h"
#include "D3D11ConstantBufferImpl.h"
#include "D3D11TexturesLoader.h"
#include "D3D11Device.h"
#include "D3D11ShadersManager.h"
#include "D3D11SamplerState.h"

namespace d3d11
{
	struct RawRenderablesPack : public renderer::IRawRenderablesPack
	{
		struct Vertices
		{
			Uint16 m_VBStride; // 2
			Uint16 m_startIndex; // 4
			Uint16 m_endIndex; // 6
		};
		static_assert( sizeof( Vertices ) == 6, "Vertices struct is larger than before!" );

		std::vector< Vertices > m_vertices;
		std::vector< ID3D11Buffer* > m_vertexBuffers;
		std::vector< ID3D11Buffer* > m_meshCBs;

		struct Shape
		{
			Uint64 m_IBOffset : 24 ;
			Uint64 m_indicesAmount : 24;

			Uint64 m_resourcesStartIndex : 11;
			Uint64 m_resourcesAmount : 5;
		};
		static_assert( sizeof( Shape ) == 8, "Shape struct is larger than before!" );

		std::vector< Shape > m_shapes;
		std::vector< ID3D11Buffer* > m_indexBuffers;
		std::vector< ID3D11VertexShader* > m_vertexShaders;
		std::vector< ID3D11PixelShader* > m_pixelShaders;
		std::vector< ID3D11Buffer* > m_materialCBs;
		std::vector< ID3D11InputLayout* > m_inputLayouts;

		std::vector< ID3D11ShaderResourceView* > m_resourceViews;
	};

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

	std::unique_ptr< renderer::IVertexBuffer > D3D11Renderer::CreateVertexBuffer( const renderer::Vertices& vertices ) const
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

		for( const renderer::IRenderTargetView* target : rendererTargetViews )
		{
			FORGE_ASSERT( dynamic_cast<const D3D11RenderTargetView*>( target ) );
			views.emplace_back( static_cast< const D3D11RenderTargetView* >( target )->GetRenderTargetView() );
		}

		m_context->GetDeviceContext()->OMSetRenderTargets( static_cast<Uint32>( views.size() ), views.data(), m_depthStencilBuffer ? m_depthStencilBuffer->GetView() : nullptr );
	}

	std::unique_ptr< renderer::ISamplerState > D3D11Renderer::CreateSamplerState( renderer::SamplerStateFilterType filterType )
	{
		return std::make_unique< d3d11::D3D11SamplerState >( *GetDevice(), filterType );
	}

	void D3D11Renderer::SetSamplerStates( const std::vector< renderer::ISamplerState* > samplerStates )
	{
		std::vector< ID3D11SamplerState* > rawStates;
		rawStates.reserve( samplerStates.size() );
		for( const renderer::ISamplerState* samplerState : samplerStates )
		{
			FORGE_ASSERT( dynamic_cast< const D3D11SamplerState* >( samplerState ) );
			rawStates.emplace_back( static_cast< const d3d11::D3D11SamplerState* >( samplerState )->GetSamplerState() );
		}

		GetContext()->GetDeviceContext()->PSSetSamplers( 0u, static_cast< Uint32 >( rawStates.size() ), rawStates.data() );
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

	std::unique_ptr< renderer::IRawRenderablesPack > D3D11Renderer::CreateRawRenderablesPackage( const std::vector< const renderer::Renderable* >& renderables ) const
	{
		auto result = std::make_unique< RawRenderablesPack >();

		for( auto& renderable : renderables )
		{
			Uint16 startIndex = static_cast< Uint16 >( result->m_shapes.size() );

			const auto& shapes = renderable->GetModel().GetShapes();
			for( const auto& shape : shapes )
			{
				const renderer::Material& material = renderable->GetMaterials()[ shape.m_materialIndex ];

				const D3D11IndexBuffer* ib = static_cast<const D3D11IndexBuffer*>( shape.m_indexBuffer.get() );
				const D3D11VertexShader* vs = static_cast<const D3D11VertexShader*>( renderable->GetMaterials()[ shape.m_materialIndex ].GetVertexShader() );
				const D3D11PixelShader* ps = static_cast<const D3D11PixelShader*>( renderable->GetMaterials()[ shape.m_materialIndex ].GetPixelShader() );
				const D3D11ConstantBufferImpl* materialCBImpl = static_cast<const D3D11ConstantBufferImpl*>( material.GetConstantBuffer()->GetImpl() );
				const D3D11InputLayout* il = static_cast<const D3D11InputLayout*>( renderable->GetMaterials()[ shape.m_materialIndex ].GetInputLayout() );

				result->m_indexBuffers.emplace_back( ib->GetBuffer() );
				result->m_vertexShaders.emplace_back( vs->GetShader() );
				result->m_pixelShaders.emplace_back( ps->GetShader() );
				result->m_materialCBs.emplace_back( materialCBImpl->GetBuffer() );
				result->m_inputLayouts.emplace_back( il->GetLayout() );

				Uint32 resourcesAmount = material.GetTexturesAmount();
				Uint32 startIndex = static_cast< Uint32 >( result->m_resourceViews.size() );
				FORGE_ASSERT( startIndex < 1 << 11 );
				FORGE_ASSERT( resourcesAmount < 1 << 5 );
				for( Uint32 resourcesIndex = 0u; resourcesIndex < resourcesAmount; ++resourcesIndex )
				{
					const D3D11Texture* texture = static_cast< const D3D11Texture* >( material.GetTexture( resourcesIndex ) );
					result->m_resourceViews.emplace_back( texture->GetShaderResourceView() );
				}

				FORGE_ASSERT( ib->GetIndicesAmount() < 1 << 24 );
				result->m_shapes.emplace_back( RawRenderablesPack::Shape{ 0u, ib->GetIndicesAmount(), Math::Min( startIndex, static_cast< Uint32 >( result->m_resourceViews.size() ) - 1u ), resourcesAmount } );
			}

			Uint16 endIndex = static_cast< Uint16 >( result->m_shapes.size() );
			const D3D11VertexBuffer* vb = static_cast<const D3D11VertexBuffer*>( renderable->GetModel().GetVertexBuffer() );
			const D3D11ConstantBufferImpl* meshCBImpl = static_cast<const D3D11ConstantBufferImpl*>( renderable->GetCBMesh().GetImpl() );

			result->m_vertexBuffers.emplace_back( vb->GetBuffer() );
			result->m_meshCBs.emplace_back( meshCBImpl->GetBuffer() );
			result->m_vertices.emplace_back( RawRenderablesPack::Vertices{ static_cast< Uint16 >( vb->GetStride() ), startIndex, endIndex } );
		}

		return result;
	}

	void D3D11Renderer::Draw( const renderer::IRawRenderablesPack& rawRenderables )
	{
		auto* context = GetContext()->GetDeviceContext();

		const RawRenderablesPack& renderables = static_cast< const RawRenderablesPack& >( rawRenderables );

		for( Uint32 verticesIndex = 0u; verticesIndex < renderables.m_vertices.size(); ++verticesIndex )
		{
			Uint32 offset = 0u;
			Uint32 stride = renderables.m_vertices[ verticesIndex ].m_VBStride;
			context->IASetVertexBuffers( 0, 1, &renderables.m_vertexBuffers[ verticesIndex ], &stride, &offset );
			context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
			context->VSSetConstantBuffers( static_cast<Uint32>( renderer::VSConstantBufferType::Mesh ), 1, &renderables.m_meshCBs[ verticesIndex ] );

			for( Uint16 shapesIndex = renderables.m_vertices[ verticesIndex ].m_startIndex; shapesIndex < renderables.m_vertices[ verticesIndex ].m_endIndex; ++shapesIndex )
			{
				const RawRenderablesPack::Shape& shape = renderables.m_shapes[ shapesIndex ];
				context->IASetIndexBuffer( renderables.m_indexBuffers[ shapesIndex ], DXGI_FORMAT_R32_UINT, renderables.m_shapes[ shapesIndex ].m_IBOffset );
				context->VSSetShader( renderables.m_vertexShaders[ shapesIndex ], 0, 0 );
				context->PSSetShader( renderables.m_pixelShaders[ shapesIndex ], 0, 0 );
				context->VSSetConstantBuffers( static_cast<Uint32>( renderer::VSConstantBufferType::Material ), 1, &renderables.m_materialCBs[ shapesIndex ] );
				context->IASetInputLayout( renderables.m_inputLayouts[ shapesIndex ] );
				context->PSSetShaderResources( 0, shape.m_resourcesAmount, &renderables.m_resourceViews[ shape.m_resourcesStartIndex ] );

				GetContext()->Draw( renderables.m_shapes[ shapesIndex ].m_indicesAmount, 0 );
			}
		}
	}

	std::unique_ptr< renderer::IConstantBufferImpl > D3D11Renderer::CreateConstantBufferImpl() const
	{
		return std::make_unique< D3D11ConstantBufferImpl >( *m_context, *m_device );
	}

	std::unique_ptr< renderer::ITexturesLoader > D3D11Renderer::CreateTexturesLoader() const
	{
		return std::make_unique< d3d11::D3D11TexturesLoader >( *GetDevice() );
	}

	void D3D11Renderer::InitializeSwapChainAndContext( const windows::WindowsWindow& window )
	{
		auto swapChainDesc = D3D11Swapchain::GetSwapChainDescription( window.GetWidth(), window.GetHeight(), window.GetHWND() );

		IDXGISwapChain* swapChain;
		ID3D11Device* d3d11Device;
		ID3D11DeviceContext* d3d11DevCon;

		Uint32 flags = 0u;
		
#ifdef FORGE_GPU_DEBUGGING
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
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
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