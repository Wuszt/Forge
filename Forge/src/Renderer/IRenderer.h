#pragma once
#include "ConstantBuffer.h"
#include "ITexture.h"

namespace forge
{
	class IWindow;
	class DepotsContainer;
	class AssetsManager;
}

namespace renderer
{
	class IRenderContext;
	class IRenderTargetView;
	class ISwapchain;
	class IVertexShader;
	class IPixelShader;
	class IVertexBuffer;
	class IInputLayout;
	class IIndexBuffer;
	class ICamera;
	class Vertices;
	class IConstantBufferImpl;
	class IDepthStencilBuffer;
	class Renderable;
	class IShadersManager;
	class ITexturesLoader;
	class ISamplerState;
	class IBlendState;
	class IDepthStencilState;
	class IDepthStencilView;
	struct ShaderDefine;
	enum class SamplerStateFilterType;
	enum class SamplerStateComparisonType;
	enum class DepthStencilComparisonFunc;
	struct BlendOperationDesc;

	enum class RendererType
	{
		D3D11,
		Unknown
	};

	enum class RenderingPass
	{
		Opaque,
		Transparent,
		Overlay,
		Count
	};

	struct IRawRenderablesPack
	{
		virtual Bool IsEmpty() const = 0;
		virtual ~IRawRenderablesPack() = default;
	};

	class RawRenderablesPacks
	{
	public:
		template< class T >
		RawRenderablesPacks( std::array< std::unique_ptr< T >, static_cast< Uint32 >( RenderingPass::Count ) >&& data )
		{
			for( Uint32 i = 0; i < static_cast<Uint32>( RenderingPass::Count ); ++i )
			{
				m_packs[ i ] = std::move( data[ i ] );
			}
		}

		const IRawRenderablesPack& GetRendenderablesPack( RenderingPass renderPass ) const
		{
			return *m_packs[ static_cast< Uint32 >( renderPass ) ];
		}

	private:
		std::unique_ptr< IRawRenderablesPack > m_packs[ static_cast<Uint32>( RenderingPass::Count ) ];
	};

	class IRenderer
	{
	public:
		static std::unique_ptr< IRenderer > CreateRenderer( const forge::DepotsContainer& depotsContainer, forge::AssetsManager& assetsManager, forge::IWindow& window, RendererType type );
		IRenderer( forge::AssetsManager& assetsManager );
		virtual ~IRenderer();

		virtual IRenderContext* GetContext() const = 0;
		virtual ISwapchain* GetSwapchain() const = 0;
		virtual IShadersManager* GetShadersManager() const = 0;

		virtual std::unique_ptr< IInputLayout > CreateInputLayout( const IVertexShader& vertexShader, const IVertexBuffer& vertexBuffer ) const = 0;
		virtual std::unique_ptr< IVertexBuffer > CreateVertexBuffer( const Vertices& vertices ) const = 0;
		virtual std::unique_ptr< IIndexBuffer > CreateIndexBuffer( const Uint32* indices, Uint32 amount ) const = 0;
		virtual std::unique_ptr< ITexture > CreateTexture( Uint32 width, Uint32 height, ITexture::Flags flags, ITexture::Format format, renderer::ITexture::Type type, ITexture::Format srvFormat = ITexture::Format::Unknown ) const = 0;
		virtual std::unique_ptr< IBlendState > CreateBlendState( const BlendOperationDesc& rgbOperation, const BlendOperationDesc& alphaDesc ) const = 0;
		virtual std::unique_ptr< renderer::IDepthStencilBuffer > CreateDepthStencilBuffer( Uint32 width, Uint32 height, Bool cubeTexture = false ) const = 0;
		virtual std::unique_ptr< IDepthStencilState > CreateDepthStencilState( DepthStencilComparisonFunc comparisonFunc ) const = 0;
		virtual std::unique_ptr< ISamplerState > CreateSamplerState( SamplerStateFilterType filterType, SamplerStateComparisonType comparisonType ) = 0;

		virtual void SetDepthBias( Float bias, Float slopeScaledBias, Float clamp ) = 0;
		virtual void SetViewportSize( const Vector2& size ) = 0;
		virtual void SetRenderTargets( const forge::ArraySpan< IRenderTargetView* >& rendererTargetViews, IDepthStencilView* depthStencilView ) = 0;
		virtual void SetSamplerStates( const forge::ArraySpan< ISamplerState* > samplerStates ) = 0;
		virtual void SetShaderResourceViews( const forge::ArraySpan< IShaderResourceView* >& input, Uint32 startIndex = 0u ) = 0;
		virtual void ClearShaderResourceViews() = 0;

		virtual void OnBeforeDraw() = 0;
		virtual RendererType GetType() const = 0;

		virtual void DrawRawVertices( Uint32 amount ) = 0;
		void Draw( const renderer::Renderable& renderable );
		virtual void Draw( const renderer::IRawRenderablesPack& rawRenderables, const ShaderDefine* shaderDefine = nullptr, forge::ArraySpan< renderer::IShaderResourceView* > additionalSRVs = {} ) = 0;

		virtual std::unique_ptr< renderer::RawRenderablesPacks > CreateRawRenderablesPackage( const forge::ArraySpan< const Renderable* >& renderables ) const = 0;

		template< class T >
		std::unique_ptr< StaticConstantBuffer< T > > CreateStaticConstantBuffer() const
		{
			auto constBuffer = std::make_unique< StaticConstantBuffer< T > >();
			constBuffer->SetImpl( CreateConstantBufferImpl() );
			return constBuffer;
		}

		std::unique_ptr< ConstantBuffer > CreateConstantBuffer() const
		{
			auto constBuffer = std::make_unique< ConstantBuffer >();
			constBuffer->SetImpl( CreateConstantBufferImpl() );
			return constBuffer;
		}

		std::unique_ptr< ConstantBuffer > CreateConstantBufferFromOther( const ConstantBuffer& data ) const
		{
			auto buff = CreateConstantBuffer();
			buff->CopyDataFrom( data );
			buff->UpdateBuffer();
			return buff;
		}

		virtual std::unique_ptr< IConstantBufferImpl > CreateConstantBufferImpl() const = 0;
	};
}

