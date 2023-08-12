#include "Fpch.h"
#include "D3D11Renderer.h"
#include "../Core/WindowsWindow.h"
#include "D3D11ConstantBufferImpl.h"
#include "D3D11TexturesLoader.h"
#include "D3D11Device.h"
#include "D3D11ShadersManager.h"
#include "D3D11SamplerState.h"
#include "D3D11BlendState.h"
#include "D3D11DepthStencilState.h"
#include "D3D11DepthStencilView.h"
#include "../Core/AssetsManager.h"
#include "../ECS/ECSManager.h"
#include "../ECS/EntityID.h"
#include "DDSTexturesLoader.h"

namespace d3d11
{
	D3D11_RASTERIZER_DESC GetDefaultRasterizerDesc()
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

		return rasterizerDesc;
	}

	void InitializeRasterizer( D3D11Device& device, D3D11RenderContext& context, const D3D11_RASTERIZER_DESC& desc, ID3D11RasterizerState** outRasterizerState )
	{
		device.GetDevice()->CreateRasterizerState( &desc, outRasterizerState );
		context.GetDeviceContext()->RSSetState( *outRasterizerState );
	}

	D3D11Renderer::D3D11Renderer( const forge::DepotsContainer& depotsContainer, forge::AssetsManager& assetsManager, forge::IWindow& window )
		: Renderer( assetsManager )
	{
		FORGE_ASSERT( dynamic_cast< windows::WindowsWindow* >( &window ) );

		InitializeSwapChainAndContext( static_cast< windows::WindowsWindow& >( window ), window.GetWidth(), window.GetHeight() );
		m_shadersManager = std::make_unique< D3D11ShadersManager >( depotsContainer, *GetDevice(), *GetContext() );

		SetViewportSize( Vector2( static_cast< Float >( window.GetWidth() ), static_cast< Float >( window.GetHeight() ) ) );

		InitializeRasterizer( *m_device, *m_context, GetDefaultRasterizerDesc(), &m_rasterizerState );

		m_windowCallbackToken = window.RegisterEventListener(
			[ & ]( const forge::IWindow::IEvent& event )
			{
				switch ( event.GetEventType() )
				{
				case forge::IWindow::EventType::OnResized:
					FORGE_ASSERT( dynamic_cast< const forge::IWindow::OnResizedEvent* >( &event ) );
					const forge::IWindow::OnResizedEvent& resizedEvent = static_cast< const forge::IWindow::OnResizedEvent& >( event );

					GetSwapchain()->Resize( resizedEvent.GetWidth(), resizedEvent.GetHeight() );
					SetViewportSize( Vector2( static_cast< Float >( window.GetWidth() ), static_cast< Float >( window.GetHeight() ) ) );
					break;
				}
			} );

		assetsManager.AddAssetsLoader< D3D11TexturesLoader >( *GetDevice(), *GetContext() );
		assetsManager.AddAssetsLoader< DDSTexturesLoader >( *GetDevice(), *GetContext() );
	}

	D3D11Renderer::~D3D11Renderer()
	{
		m_rasterizerState->Release();
	}

	std::unique_ptr< renderer::IInputLayout > D3D11Renderer::CreateInputLayout( const renderer::IVertexShader& vertexShader, const renderer::IVertexBuffer& vertexBuffer ) const
	{
		FORGE_ASSERT( dynamic_cast< const D3D11VertexShader* >( &vertexShader ) );
		return std::make_unique< D3D11InputLayout >( GetContext(), *GetDevice(), static_cast< const D3D11VertexShader& >( vertexShader ), static_cast< const D3D11VertexBuffer& >( vertexBuffer ) );
	}

	std::unique_ptr< renderer::IVertexBuffer > D3D11Renderer::CreateVertexBuffer( const renderer::Vertices& vertices ) const
	{
		return std::make_unique< D3D11VertexBuffer >( GetContext(), *GetDevice(), vertices );
	}

	std::unique_ptr< renderer::IIndexBuffer > D3D11Renderer::CreateIndexBuffer( const Uint32* indices, Uint32 amount ) const
	{
		return std::make_unique< D3D11IndexBuffer >( GetContext(), *GetDevice(), indices, amount );
	}

	std::unique_ptr< renderer::ITexture > D3D11Renderer::CreateTexture( Uint32 width, Uint32 height, renderer::ITexture::Flags flags, renderer::ITexture::Format format, renderer::ITexture::Type type, renderer::ITexture::Format srvFormat /*= renderer::ITexture::Format::Unknown */ ) const
	{
		return std::make_unique< D3D11Texture >( *GetDevice(), *GetContext(), width, height, flags, format, type, srvFormat );
	}

	std::unique_ptr< renderer::IBlendState > D3D11Renderer::CreateBlendState( const renderer::BlendOperationDesc& rgbOperation, const renderer::BlendOperationDesc& alphaDesc ) const
	{
		return std::make_unique< d3d11::D3D11BlendState >( *GetDevice(), *GetContext(), rgbOperation, alphaDesc );
	}

	std::unique_ptr< renderer::IDepthStencilBuffer > D3D11Renderer::CreateDepthStencilBuffer( Uint32 width, Uint32 height, Bool cubeTexture ) const
	{
		return std::make_unique< D3D11DepthStencilBuffer >( *GetDevice(), *GetContext(), width, height, cubeTexture );
	}

	std::unique_ptr< renderer::IDepthStencilState > D3D11Renderer::CreateDepthStencilState( renderer::DepthStencilComparisonFunc comparisonFunc ) const
	{
		return std::make_unique< d3d11::D3D11DepthStencilState >( *GetDevice(), *GetContext(), comparisonFunc );
	}

	void D3D11Renderer::SetRenderTargets( const forge::ArraySpan< renderer::IRenderTargetView* >& rendererTargetViews, renderer::IDepthStencilView* depthStencilView )
	{
		FORGE_ASSERT( depthStencilView == nullptr || dynamic_cast< D3D11DepthStencilView* >( depthStencilView ) );
		SetRenderTargets( rendererTargetViews, static_cast< D3D11DepthStencilView* >( depthStencilView ) );
	}

	void D3D11Renderer::SetRenderTargets( const forge::ArraySpan< renderer::IRenderTargetView* >& rendererTargetViews, D3D11DepthStencilView* depthStencilView )
	{
		std::vector< ID3D11RenderTargetView* > views;
		views.reserve( rendererTargetViews.GetSize() );

		for ( const renderer::IRenderTargetView* target : rendererTargetViews )
		{
			FORGE_ASSERT( target == nullptr || dynamic_cast< const D3D11RenderTargetView* >( target ) );
			views.emplace_back( target ? static_cast< const D3D11RenderTargetView* >( target )->GetRenderTargetView() : nullptr );
		}

		D3D11DepthStencilView* dsv = static_cast< D3D11DepthStencilView* >( depthStencilView );

		m_context->GetDeviceContext()->OMSetRenderTargets( static_cast< Uint32 >( views.size() ), views.data(), dsv ? &dsv->GetView() : nullptr );
	}

	std::unique_ptr< renderer::ISamplerState > D3D11Renderer::CreateSamplerState( renderer::SamplerStateFilterType filterType, renderer::SamplerStateComparisonType comparisonType )
	{
		return std::make_unique< d3d11::D3D11SamplerState >( *GetDevice(), filterType, comparisonType );
	}

	void D3D11Renderer::SetCullingMode( renderer::CullingMode mode )
	{
		D3D11_RASTERIZER_DESC desc;
		m_rasterizerState->GetDesc( &desc );

		switch ( mode )
		{
		case renderer::CullingMode::None:
			desc.CullMode = D3D11_CULL_NONE;
			break;
		case renderer::CullingMode::CullingBack:
			desc.CullMode = D3D11_CULL_BACK;
			break;
		case renderer::CullingMode::CullingFront:
			desc.CullMode = D3D11_CULL_FRONT;
			break;
		default:
			FORGE_FATAL( "Unknown mode" );
		}

		InitializeRasterizer( *m_device, *m_context, desc, &m_rasterizerState );
	}

	renderer::CullingMode D3D11Renderer::GetCullingMode() const
	{
		D3D11_RASTERIZER_DESC desc;
		m_rasterizerState->GetDesc( &desc );

		switch ( desc.CullMode )
		{
		case D3D11_CULL_NONE:
			return renderer::CullingMode::None;
		case D3D11_CULL_BACK:
			return renderer::CullingMode::CullingBack;
		case D3D11_CULL_FRONT:
			return renderer::CullingMode::CullingFront;
		default:
			FORGE_FATAL( "Unknown mode" );
		}

		return renderer::CullingMode::None;
	}

	void D3D11Renderer::SetFillMode( renderer::FillMode mode )
	{
		D3D11_RASTERIZER_DESC desc;
		m_rasterizerState->GetDesc( &desc );

		switch ( mode )
		{
		case renderer::FillMode::Solid:
			desc.FillMode = D3D11_FILL_SOLID;
			break;
		case renderer::FillMode::WireFrame:
			desc.FillMode = D3D11_FILL_WIREFRAME;
			break;
		default:
			FORGE_FATAL( "Unknown mode" );
		}

		InitializeRasterizer( *m_device, *m_context, desc, &m_rasterizerState );
	}

	renderer::FillMode D3D11Renderer::GetFillMode() const
	{
		D3D11_RASTERIZER_DESC desc;
		m_rasterizerState->GetDesc( &desc );

		switch ( desc.FillMode )
		{
		case D3D11_FILL_SOLID:
			return renderer::FillMode::Solid;
		case D3D11_FILL_WIREFRAME:
			return renderer::FillMode::WireFrame;
		default:
			FORGE_FATAL( "Unknown mode" );
		}

		return renderer::FillMode::Solid;
	}

	void D3D11Renderer::SetDepthBias( Int32 bias, Float slopeScaledBias, Float clamp )
	{
		D3D11_RASTERIZER_DESC desc;
		m_rasterizerState->GetDesc( &desc );
		desc.DepthBias = bias;
		desc.SlopeScaledDepthBias = slopeScaledBias;
		desc.DepthBiasClamp = clamp;
		InitializeRasterizer( *m_device, *m_context, desc, &m_rasterizerState );
	}

	void D3D11Renderer::SetSamplerStates( const forge::ArraySpan< renderer::ISamplerState* > samplerStates )
	{
		std::vector< ID3D11SamplerState* > rawStates;
		rawStates.reserve( samplerStates.GetSize() );
		for ( const renderer::ISamplerState* samplerState : samplerStates )
		{
			FORGE_ASSERT( dynamic_cast< const D3D11SamplerState* >( samplerState ) );
			rawStates.emplace_back( static_cast< const d3d11::D3D11SamplerState* >( samplerState )->GetSamplerState() );
		}

		GetContext()->GetDeviceContext()->VSSetSamplers( 0u, static_cast< Uint32 >( rawStates.size() ), rawStates.data() );
		GetContext()->GetDeviceContext()->PSSetSamplers( 0u, static_cast< Uint32 >( rawStates.size() ), rawStates.data() );
	}

	void D3D11Renderer::SetShaderResourceViews( const forge::ArraySpan< const renderer::IShaderResourceView* >& input, Uint32 startIndex )
	{
		std::vector< ID3D11ShaderResourceView* > srvs;
		srvs.reserve( input.GetSize() );

		for ( const auto srv : input )
		{
			FORGE_ASSERT( srv == nullptr || dynamic_cast< const D3D11ShaderResourceView* >( srv ) );
			srvs.emplace_back( srv ? static_cast< const D3D11ShaderResourceView* >( srv )->GetTypedSRV() : nullptr );
		}

		GetContext()->GetDeviceContext()->VSSetShaderResources( startIndex, static_cast< Uint32 >( srvs.size() ), srvs.data() );
		GetContext()->GetDeviceContext()->PSSetShaderResources( startIndex, static_cast< Uint32 >( srvs.size() ), srvs.data() );
	}

	void D3D11Renderer::ClearShaderResourceViews()
	{
		ID3D11ShaderResourceView* srvs[ D3D11_STANDARD_VERTEX_ELEMENT_COUNT ]{ nullptr };
		GetContext()->GetDeviceContext()->VSSetShaderResources( 0, static_cast< Uint32 >( D3D11_STANDARD_VERTEX_ELEMENT_COUNT ), srvs );
		GetContext()->GetDeviceContext()->PSSetShaderResources( 0, static_cast< Uint32 >( D3D11_STANDARD_VERTEX_ELEMENT_COUNT ), srvs );
	}

	void D3D11Renderer::OnBeforeDraw()
	{
		struct cbFrame
		{
			Float Time;
			Float padding[ 3 ];
		};

		auto buff = CreateStaticConstantBuffer< cbFrame >();

		buff->GetData().Time = forge::Time::GetTime();
		buff->UpdateBuffer();
		buff->SetVS( renderer::VSConstantBufferType::Frame );
		buff->SetPS( renderer::PSConstantBufferType::Frame );
	}

	void D3D11Renderer::SetViewportSize( const Vector2& size )
	{
		D3D11_VIEWPORT viewport;
		ZeroMemory( &viewport, sizeof( D3D11_VIEWPORT ) );

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = size.X;
		viewport.Height = size.Y;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		m_context->GetDeviceContext()->RSSetViewports( 1, &viewport );
	}

	struct RawRenderableFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( RawRenderableFragment, ecs::Fragment );

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_meshCB = nullptr;
		Uint32 m_vbStride = 0u;

		struct Shape
		{
			ID3D11Buffer* m_indexBuffer = nullptr;
			Uint32 m_IBOffset = 0u;
			Uint32 m_indicesAmount = 0u;

			ID3D11Buffer* m_materialCB;
			ID3D11InputLayout* m_inputLayout = nullptr;

			struct Shaders
			{
				ID3D11VertexShader* m_vertexShader;
				ID3D11PixelShader* m_pixelShader;
			};

			std::vector< Shaders > m_shaders;
			std::vector< ID3D11ShaderResourceView* > m_resourceViews;
		};


		template< class T >
		struct AdditionalCB
		{
			T m_type;
			ID3D11Buffer* m_cb;
		};

		static constexpr Uint32 c_maxAdditionalCBsAmount = 4u;
		AdditionalCB< renderer::VSConstantBufferType > m_additionalVSCBs[ c_maxAdditionalCBsAmount ];
		AdditionalCB< renderer::PSConstantBufferType > m_additionalPSCBs[ c_maxAdditionalCBsAmount ];
		std::array< std::vector< Shape >, static_cast< Uint32 >( renderer::RenderingPass::Count ) > m_shapes;
	};

	RTTI_IMPLEMENT_TYPE( d3d11::RawRenderableFragment );

	const ecs::Fragment::Type& D3D11Renderer::GetECSFragmentType() const
	{
		return RawRenderableFragment::GetTypeStatic();
	}

	void D3D11Renderer::Draw( const ecs::Archetype& archetype, renderer::RenderingPass renderingPass, const renderer::ShaderDefine* shaderDefine /*= nullptr*/, forge::ArraySpan< const renderer::IShaderResourceView* > additionalSRVs /*= {} */ )
	{
		auto* context = GetContext()->GetDeviceContext();
		context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		if ( !additionalSRVs.IsEmpty() )
		{
			SetShaderResourceViews( additionalSRVs, D3D11_STANDARD_VERTEX_ELEMENT_COUNT - additionalSRVs.GetSize() ); //todo: this is shit, srvs should be next to each other
		}

		auto fragments = archetype.GetFragments< RawRenderableFragment >();
		for ( const auto& fragment : fragments )
		{
			Draw_Internal( fragment, renderingPass, shaderDefine );
		}
	}

	void D3D11Renderer::Draw_Internal( const RawRenderableFragment& fragment, renderer::RenderingPass renderingPass, const renderer::ShaderDefine* shaderDefine /*= nullptr */ )
	{
		auto* context = GetContext()->GetDeviceContext();

		Uint32 shadersIndex = 0u;

		if ( shaderDefine )
		{
			auto it = std::find_if( GetShadersManager()->GetBaseShaderDefines().begin(), GetShadersManager()->GetBaseShaderDefines().end(), [ &shaderDefine ]( const renderer::ShaderDefine& sd ) { return sd.GetHash() == shaderDefine->GetHash(); } );
			shadersIndex = static_cast< Uint32 >( it - GetShadersManager()->GetBaseShaderDefines().begin() + 1u );
		}

		const RawRenderableFragment& renderableFragment = static_cast< const RawRenderableFragment& >( fragment );
		constexpr Uint32 offset = 0u;
		context->IASetVertexBuffers( 0, 1, &renderableFragment.m_vertexBuffer, &renderableFragment.m_vbStride, &offset );

		context->VSSetConstantBuffers( static_cast< Uint32 >( renderer::VSConstantBufferType::Mesh ), 1, &renderableFragment.m_meshCB );
		context->PSSetConstantBuffers( static_cast< Uint32 >( renderer::PSConstantBufferType::Mesh ), 1, &renderableFragment.m_meshCB );

		for ( const auto& cb : renderableFragment.m_additionalVSCBs )
		{
			context->VSSetConstantBuffers( static_cast< Uint32 >( cb.m_type ), 1, &cb.m_cb );
		}

		for ( const auto& cb : renderableFragment.m_additionalPSCBs )
		{
			context->PSSetConstantBuffers( static_cast< Uint32 >( cb.m_type ), 1, &cb.m_cb );
		}

		for ( const RawRenderableFragment::Shape& shape : renderableFragment.m_shapes[ static_cast< Uint32 >( renderingPass ) ] )
		{
			context->IASetIndexBuffer( shape.m_indexBuffer, DXGI_FORMAT_R32_UINT, shape.m_IBOffset );
			context->VSSetShader( shape.m_shaders[ shadersIndex ].m_vertexShader, 0, 0 );
			context->PSSetShader( shape.m_shaders[ shadersIndex ].m_pixelShader, 0, 0 );

			ID3D11Buffer* materialCB = shape.m_materialCB;
			context->VSSetConstantBuffers( static_cast< Uint32 >( renderer::VSConstantBufferType::Material ), 1, &materialCB );
			context->PSSetConstantBuffers( static_cast< Uint32 >( renderer::PSConstantBufferType::Material ), 1, &materialCB );
			context->IASetInputLayout( shape.m_inputLayout );

			context->VSSetShaderResources( 0, static_cast< Uint32 >( shape.m_resourceViews.size() ), shape.m_resourceViews.data() );
			context->PSSetShaderResources( 0, static_cast< Uint32 >( shape.m_resourceViews.size() ), shape.m_resourceViews.data() );

			GetContext()->Draw( shape.m_indicesAmount, 0 );
		}
	}

	void D3D11Renderer::UpdateRenderableECSFragment( ecs::ECSManager& ecsManager, ecs::EntityID entityID, const renderer::Renderable& renderable ) const
	{
		const auto& shapes = renderable.GetModel().GetShapes();

		RawRenderableFragment* rawRenderable = ecsManager.GetFragment< RawRenderableFragment >( entityID );

		*rawRenderable = RawRenderableFragment();

		for ( const auto& shape : shapes )
		{
			const renderer::Material& material = *renderable.GetMaterials()[ shape.m_materialIndex ].get();
			auto& rawShape = rawRenderable->m_shapes[ static_cast< Uint32 >( material.GetRenderingPass() ) ].emplace_back();

			const D3D11IndexBuffer* ib = static_cast< const D3D11IndexBuffer* >( shape.m_indexBuffer.get() );
			rawShape.m_indexBuffer = ib->GetBuffer();
			rawShape.m_indicesAmount = ib->GetIndicesAmount();

			const D3D11ConstantBufferImpl* materialCBImpl = static_cast< const D3D11ConstantBufferImpl* >( material.GetConstantBuffer()->GetImpl() );
			rawShape.m_materialCB = materialCBImpl->GetBuffer();

			const D3D11InputLayout* il = static_cast< const D3D11InputLayout* >( material.GetInputLayout() );
			rawShape.m_inputLayout = il->GetLayout();

			{
				auto vertexShaderPack = material.GetVertexShader();
				auto pixelShaderPack = material.GetPixelShader();

				const D3D11VertexShader* mainVs = static_cast< const D3D11VertexShader* >( vertexShaderPack->GetMainShader().get() );
				const D3D11PixelShader* mainPs = static_cast< const D3D11PixelShader* >( pixelShaderPack->GetMainShader().get() );

				rawShape.m_shaders.push_back( { mainVs->GetShader(), mainPs->GetShader() } );

				for ( const auto& define : GetShadersManager()->GetBaseShaderDefines() )
				{
					const D3D11VertexShader* vs = static_cast< const D3D11VertexShader* >( vertexShaderPack->GetSubShaders().at( define ).get() );
					const D3D11PixelShader* ps = static_cast< const D3D11PixelShader* >( pixelShaderPack->GetSubShaders().at( define ).get() );

					rawShape.m_shaders.push_back( { vs->GetShader(), ps->GetShader() } );
				}
			}

			auto materialResources = material.GetTextures();
			Uint32 resourcesAmount = materialResources.GetSize();
			for ( Uint32 i = 0u; i < resourcesAmount; ++i )
			{
				const D3D11Texture* texture = static_cast< const D3D11Texture* >( materialResources[ i ].get() );
				rawShape.m_resourceViews.emplace_back( texture ? texture->GetShaderResourceView()->GetTypedSRV() : nullptr );
			}
		}

		const D3D11VertexBuffer* vb = static_cast< const D3D11VertexBuffer* >( renderable.GetModel().GetVertexBuffer() );
		const D3D11ConstantBufferImpl* meshCBImpl = static_cast< const D3D11ConstantBufferImpl* >( renderable.GetCBMesh().GetImpl() );

		rawRenderable->m_meshCB = meshCBImpl->GetBuffer();
		rawRenderable->m_vertexBuffer = vb->GetBuffer();
		rawRenderable->m_vbStride = vb->GetStride();

		Uint32 psCBsAmount = 0u;
		Uint32 vsCBsAmount = 0u;
		for ( auto cb : renderable.GetAdditionalCBs() )
		{
			const D3D11ConstantBufferImpl* cbImpl = static_cast< const D3D11ConstantBufferImpl* >( cb.m_cbImpl );

			if ( cb.m_psBufferType != renderer::PSConstantBufferType::Invalid )
			{
				FORGE_ASSERT( psCBsAmount < RawRenderableFragment::c_maxAdditionalCBsAmount );
				rawRenderable->m_additionalPSCBs[ psCBsAmount++ ] = { cb.m_psBufferType, cbImpl->GetBuffer() };
			}

			if ( cb.m_vsBufferType != renderer::VSConstantBufferType::Invalid )
			{
				FORGE_ASSERT( vsCBsAmount < RawRenderableFragment::c_maxAdditionalCBsAmount );
				rawRenderable->m_additionalVSCBs[ vsCBsAmount++ ] = { cb.m_vsBufferType, cbImpl->GetBuffer() };
			}
		}
	}

	void D3D11Renderer::DrawRawVertices( Uint32 amount )
	{
		GetContext()->GetDeviceContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

		GetContext()->GetDeviceContext()->Draw( amount, 0u );
	}

	std::unique_ptr< renderer::IConstantBufferImpl > D3D11Renderer::CreateConstantBufferImpl() const
	{
		return std::make_unique< D3D11ConstantBufferImpl >( *m_device, *m_context );
	}

	void D3D11Renderer::InitializeSwapChainAndContext( const windows::WindowsWindow& window, Uint32 renderingResolutionWidth, Uint32 renderingResolutionHeight )
	{
		auto swapChainDesc = D3D11Swapchain::GetSwapChainDescription( renderingResolutionWidth, renderingResolutionHeight, window.GetHWND() );

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
		m_swapChain = std::make_unique< D3D11Swapchain >( *m_device, *m_context, swapChain );
	}
}