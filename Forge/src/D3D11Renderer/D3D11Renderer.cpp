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

		struct Shaders
		{
			std::vector< ID3D11VertexShader* > m_vertexShaders;
			std::vector< ID3D11PixelShader* > m_pixelShaders;
		};

		std::vector< Shape > m_shapes;
		std::vector< ID3D11Buffer* > m_indexBuffers;
		Shaders m_mainShaders;
		std::unordered_map< renderer::ShaderDefine, Shaders > m_subShaders;
		std::vector< ID3D11Buffer* > m_materialCBs;
		std::vector< ID3D11InputLayout* > m_inputLayouts;

		std::vector< ID3D11ShaderResourceView* > m_resourceViews;

		virtual Bool IsEmpty() const override
		{
			return m_shapes.empty();
		}
	};

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
		: IRenderer( assetsManager )
	{
		FORGE_ASSERT( dynamic_cast<windows::WindowsWindow*>( &window ) );

		InitializeSwapChainAndContext( static_cast<windows::WindowsWindow&>( window ), window.GetWidth(), window.GetHeight() );
		m_shadersManager = std::make_unique< D3D11ShadersManager >( depotsContainer, *GetDevice(), *GetContext() );

		SetViewportSize( Vector2( static_cast< Float >( window.GetWidth() ), static_cast< Float >( window.GetHeight() ) ) );

		InitializeRasterizer( *m_device, *m_context, GetDefaultRasterizerDesc(), &m_rasterizerState );

		m_windowCallbackToken = window.RegisterEventListener(
			[ & ]( const forge::IWindow::IEvent& event )
		{
			switch( event.GetEventType() )
			{
			case forge::IWindow::EventType::OnResized:
				FORGE_ASSERT( dynamic_cast<const forge::IWindow::OnResizedEvent*>( &event ) );
				const forge::IWindow::OnResizedEvent& resizedEvent = static_cast<const forge::IWindow::OnResizedEvent&>( event );

				GetSwapchain()->Resize( resizedEvent.GetWidth(), resizedEvent.GetHeight() );
				SetViewportSize( Vector2( static_cast<Float>( window.GetWidth() ), static_cast<Float>( window.GetHeight() ) ) );
				break;
			}
		} );

		assetsManager.AddAssetsLoader< D3D11TexturesLoader >( *GetDevice(), *GetContext() );
	}

	D3D11Renderer::~D3D11Renderer()
	{
		m_rasterizerState->Release();
	}

	std::unique_ptr< renderer::IInputLayout > D3D11Renderer::CreateInputLayout( const renderer::IVertexShader& vertexShader, const renderer::IVertexBuffer& vertexBuffer ) const
	{
		FORGE_ASSERT( dynamic_cast<const D3D11VertexShader*>( &vertexShader ) );
		return std::make_unique< D3D11InputLayout >( GetContext(), *GetDevice(), static_cast<const D3D11VertexShader&>( vertexShader ), static_cast<const D3D11VertexBuffer&>( vertexBuffer ) );
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
		FORGE_ASSERT( depthStencilView == nullptr || dynamic_cast<D3D11DepthStencilView*>( depthStencilView ) );
		SetRenderTargets( rendererTargetViews, static_cast<D3D11DepthStencilView*>( depthStencilView ) );
	}

	void D3D11Renderer::SetRenderTargets( const forge::ArraySpan< renderer::IRenderTargetView* >& rendererTargetViews, D3D11DepthStencilView* depthStencilView )
	{
		std::vector< ID3D11RenderTargetView* > views;
		views.reserve( rendererTargetViews.GetSize() );

		for( const renderer::IRenderTargetView* target : rendererTargetViews )
		{
			FORGE_ASSERT( target == nullptr || dynamic_cast<const D3D11RenderTargetView*>( target ) );
			views.emplace_back( target ? static_cast< const D3D11RenderTargetView* >( target )->GetRenderTargetView() : nullptr );
		}

		D3D11DepthStencilView* dsv = static_cast< D3D11DepthStencilView* >( depthStencilView );

		m_context->GetDeviceContext()->OMSetRenderTargets( static_cast<Uint32>( views.size() ), views.data(), dsv ? &dsv->GetView() : nullptr );
	}

	std::unique_ptr< renderer::ISamplerState > D3D11Renderer::CreateSamplerState( renderer::SamplerStateFilterType filterType, renderer::SamplerStateComparisonType comparisonType )
	{
		return std::make_unique< d3d11::D3D11SamplerState >( *GetDevice(), filterType, comparisonType );
	}

	void D3D11Renderer::SetDepthBias( Float bias, Float slopeScaledBias, Float clamp )
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
		for( const renderer::ISamplerState* samplerState : samplerStates )
		{
			FORGE_ASSERT( dynamic_cast< const D3D11SamplerState* >( samplerState ) );
			rawStates.emplace_back( static_cast< const d3d11::D3D11SamplerState* >( samplerState )->GetSamplerState() );
		}

		GetContext()->GetDeviceContext()->VSSetSamplers( 0u, static_cast<Uint32>( rawStates.size() ), rawStates.data() );
		GetContext()->GetDeviceContext()->PSSetSamplers( 0u, static_cast< Uint32 >( rawStates.size() ), rawStates.data() );
	}

	void D3D11Renderer::SetShaderResourceViews( const forge::ArraySpan< renderer::IShaderResourceView* >& input, Uint32 startIndex )
	{
		std::vector< ID3D11ShaderResourceView* > srvs;
		srvs.reserve( input.GetSize() );

		for( const auto srv : input )
		{
			FORGE_ASSERT( dynamic_cast<const D3D11ShaderResourceView *>( srv ) );
			srvs.emplace_back( static_cast< const D3D11ShaderResourceView* >( srv )->GetTypedSRV() );
		}

		GetContext()->GetDeviceContext()->VSSetShaderResources( startIndex, static_cast<Uint32>( srvs.size() ), srvs.data() );
		GetContext()->GetDeviceContext()->PSSetShaderResources( startIndex, static_cast< Uint32 >( srvs.size() ), srvs.data() );
	}

	void D3D11Renderer::ClearShaderResourceViews()
	{
		ID3D11ShaderResourceView* srvs[ D3D11_STANDARD_VERTEX_ELEMENT_COUNT ] { nullptr };
		GetContext()->GetDeviceContext()->VSSetShaderResources( 0, static_cast<Uint32>( D3D11_STANDARD_VERTEX_ELEMENT_COUNT ), srvs );
		GetContext()->GetDeviceContext()->PSSetShaderResources( 0, static_cast<Uint32>( D3D11_STANDARD_VERTEX_ELEMENT_COUNT ), srvs );
	}

	void D3D11Renderer::OnBeforeDraw()
	{
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

	std::unique_ptr< renderer::RawRenderablesPacks > D3D11Renderer::CreateRawRenderablesPackage( const forge::ArraySpan< const renderer::Renderable* >& renderables ) const
	{
		constexpr Uint32 packsAmount = static_cast< Uint16 >( renderer::RenderingPass::Count );
		std::array< std::unique_ptr< RawRenderablesPack >, packsAmount > packs;

		for( auto& pack : packs )
		{
			pack = std::make_unique< RawRenderablesPack >();
		}

		for( auto& renderable : renderables )
		{
			const auto& shapes = renderable->GetModel().GetShapes();

			std::array< Uint16, packsAmount > startIndices;
			for( Uint32 i = 0; i < packsAmount; ++i )
			{
				startIndices[ i ] = static_cast<Uint16>( packs[i]->m_shapes.size() );
			}

			for( const auto& shape : shapes )
			{
				const renderer::Material& material = *renderable->GetMaterials()[ shape.m_materialIndex ].get();

				Uint32 packIndex = static_cast<Uint32>( material.GetRenderingPass() );
				auto& pack = packs[ packIndex ];

				const D3D11IndexBuffer* ib = static_cast<const D3D11IndexBuffer*>( shape.m_indexBuffer.get() );
				const D3D11ConstantBufferImpl* materialCBImpl = static_cast<const D3D11ConstantBufferImpl*>( material.GetConstantBuffer()->GetImpl() );
				const D3D11InputLayout* il = static_cast<const D3D11InputLayout*>( material.GetInputLayout() );

				pack->m_indexBuffers.emplace_back( ib->GetBuffer() );
				pack->m_materialCBs.emplace_back( materialCBImpl->GetBuffer() );
				pack->m_inputLayouts.emplace_back( il->GetLayout() );

				{
					auto vertexShaderPack = material.GetVertexShader();
					auto pixelShaderPack = material.GetPixelShader();

					const D3D11VertexShader* mainVs = static_cast<const D3D11VertexShader* >( vertexShaderPack->GetMainShader().get() );
					const D3D11PixelShader* mainPs = static_cast<const D3D11PixelShader* >( pixelShaderPack->GetMainShader().get() );

					pack->m_mainShaders.m_vertexShaders.emplace_back( mainVs->GetShader() );
					pack->m_mainShaders.m_pixelShaders.emplace_back( mainPs->GetShader() );

					for( const auto& define : GetShadersManager()->GetBaseShaderDefines() )
					{
						const D3D11VertexShader* vs = static_cast<const D3D11VertexShader*>( vertexShaderPack->GetSubShaders().at( define ).get() );
						const D3D11PixelShader* ps = static_cast<const D3D11PixelShader*>( pixelShaderPack->GetSubShaders().at( define ).get() );

						pack->m_subShaders[ define ].m_vertexShaders.emplace_back( vs->GetShader() );
						pack->m_subShaders[ define ].m_pixelShaders.emplace_back( ps->GetShader() );
					}
				}

				auto materialResources = material.GetTextures();
				Uint32 resourcesAmount = materialResources.GetSize();
				Uint32 startIndex = static_cast< Uint32 >( pack->m_resourceViews.size() );
				FORGE_ASSERT( startIndex < ( 1 << 11 ) );
				FORGE_ASSERT( resourcesAmount < ( 1 << 5 ) );
				for( Uint32 resourcesIndex = 0u; resourcesIndex < resourcesAmount; ++resourcesIndex )
				{
					const D3D11Texture* texture = static_cast< const D3D11Texture* >( materialResources[ resourcesIndex ].get() );
					pack->m_resourceViews.emplace_back( texture ? texture->GetShaderResourceView()->GetTypedSRV() : nullptr );
				}

				pack->m_resourceViews.resize( Math::Max( 1u, static_cast< Uint32 >( pack->m_resourceViews.size() ) ) );

				FORGE_ASSERT( ib->GetIndicesAmount() < 1 << 24 );
				pack->m_shapes.emplace_back( RawRenderablesPack::Shape{ 0u, ib->GetIndicesAmount(), Math::Min( startIndex, static_cast< Uint32 >( pack->m_resourceViews.size() ) - 1u ), resourcesAmount } );
			}

			for( Uint32 i = 0; i < packsAmount; ++i )
			{
				Uint16 endIndex = static_cast<Uint16>( packs[ i ]->m_shapes.size() );
				if( endIndex > startIndices[ i ] )
				{
					const D3D11VertexBuffer* vb = static_cast<const D3D11VertexBuffer*>( renderable->GetModel().GetVertexBuffer() );
					const D3D11ConstantBufferImpl* meshCBImpl = static_cast<const D3D11ConstantBufferImpl*>( renderable->GetCBMesh().GetImpl() );

					packs[ i ]->m_vertexBuffers.emplace_back( vb->GetBuffer() );
					packs[ i ]->m_meshCBs.emplace_back( meshCBImpl->GetBuffer() );
					packs[ i ]->m_vertices.emplace_back( RawRenderablesPack::Vertices{ static_cast<Uint16>( vb->GetStride() ), startIndices[ i ], endIndex } );
				}
			}
		}

		return std::make_unique< renderer::RawRenderablesPacks >( std::move( packs ) );
	}

	void D3D11Renderer::Draw( const renderer::IRawRenderablesPack& rawRenderables, const renderer::ShaderDefine* shaderDefine, forge::ArraySpan< renderer::IShaderResourceView* > additionalSRVs )
	{
		if( rawRenderables.IsEmpty() )
		{
			return;
		}

		auto* context = GetContext()->GetDeviceContext();
		context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		if( !additionalSRVs.IsEmpty() )
		{
			SetShaderResourceViews( additionalSRVs, D3D11_STANDARD_VERTEX_ELEMENT_COUNT - additionalSRVs.GetSize() ); //todo: this is shit, srvs should be next to each other
		}

		const RawRenderablesPack& renderables = static_cast< const RawRenderablesPack& >( rawRenderables );

		const RawRenderablesPack::Shaders* shaders = &renderables.m_mainShaders;

		if( shaderDefine )
		{
			shaders = &renderables.m_subShaders.at( *shaderDefine );
		}

		for( Uint32 verticesIndex = 0u; verticesIndex < renderables.m_vertices.size(); ++verticesIndex )
		{
			Uint32 offset = 0u;
			Uint32 stride = renderables.m_vertices[ verticesIndex ].m_VBStride;
			context->IASetVertexBuffers( 0, 1, &renderables.m_vertexBuffers[ verticesIndex ], &stride, &offset );

			context->VSSetConstantBuffers( static_cast<Uint32>( renderer::VSConstantBufferType::Mesh ), 1, &renderables.m_meshCBs[ verticesIndex ] );
			context->PSSetConstantBuffers( static_cast<Uint32>( renderer::PSConstantBufferType::Mesh ), 1, &renderables.m_meshCBs[ verticesIndex ] );

			for( Uint16 shapesIndex = renderables.m_vertices[ verticesIndex ].m_startIndex; shapesIndex < renderables.m_vertices[ verticesIndex ].m_endIndex; ++shapesIndex )
			{
				const RawRenderablesPack::Shape& shape = renderables.m_shapes[ shapesIndex ];
				context->IASetIndexBuffer( renderables.m_indexBuffers[ shapesIndex ], DXGI_FORMAT_R32_UINT, renderables.m_shapes[ shapesIndex ].m_IBOffset );
				context->VSSetShader( shaders->m_vertexShaders[ shapesIndex ], 0, 0 );
				context->PSSetShader( shaders->m_pixelShaders[ shapesIndex ], 0, 0 );
				context->VSSetConstantBuffers( static_cast<Uint32>( renderer::VSConstantBufferType::Material ), 1, &renderables.m_materialCBs[ shapesIndex ] );
				context->PSSetConstantBuffers( static_cast<Uint32>( renderer::PSConstantBufferType::Material ), 1, &renderables.m_materialCBs[ shapesIndex ] );
				context->IASetInputLayout( renderables.m_inputLayouts[ shapesIndex ] );
				context->VSSetShaderResources( 0, shape.m_resourcesAmount, &renderables.m_resourceViews[ shape.m_resourcesStartIndex ] );
				context->PSSetShaderResources( 0, shape.m_resourcesAmount, &renderables.m_resourceViews[ shape.m_resourcesStartIndex ] );

				GetContext()->Draw( renderables.m_shapes[ shapesIndex ].m_indicesAmount, 0 );
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