#pragma once
#include "ConstantBuffer.h"
#include "ITexture.h"
#include "../ECS/Fragment.h"
#include "../ECS/Tag.h"

namespace forge
{
	class IWindow;
	class DepotsContainer;
	class AssetsManager;
}

namespace ecs
{
	class EntityID;
	class ArchetypeView;
	class MutableArchetypeView;
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
	class ShadersManager;
	class ITexturesLoader;
	class ISamplerState;
	class IBlendState;
	class IDepthStencilState;
	class IDepthStencilView;
	class ShaderDefine;
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
		Count
	};

	enum class CullingMode
	{
		None,
		CullingBack,
		CullingFront
	};

	enum class FillMode
	{
		Solid,
		WireFrame
	};

	struct WireFrameTag : public ecs::Tag
	{
		RTTI_DECLARE_STRUCT( WireFrameTag, ecs::Tag );
	};

	class Renderer
	{
	public:
		static std::unique_ptr< Renderer > CreateRenderer( const forge::DepotsContainer& depotsContainer, forge::AssetsManager& assetsManager, forge::IWindow& window, RendererType type );
		Renderer( forge::AssetsManager& assetsManager );
		virtual ~Renderer() = 0;

		virtual IRenderContext* GetContext() const = 0;
		virtual ISwapchain* GetSwapchain() const = 0;
		virtual ShadersManager* GetShadersManager() const = 0;

		virtual std::unique_ptr< IInputLayout > CreateInputLayout( const IVertexShader& vertexShader, const IVertexBuffer& vertexBuffer ) const = 0;
		virtual std::unique_ptr< IVertexBuffer > CreateVertexBuffer( const Vertices& vertices ) const = 0;
		virtual std::unique_ptr< IIndexBuffer > CreateIndexBuffer( const Uint32* indices, Uint32 amount ) const = 0;
		virtual std::unique_ptr< ITexture > CreateTexture( Uint32 width, Uint32 height, ITexture::Flags flags, ITexture::Format format, renderer::ITexture::Type type, ITexture::Format srvFormat = ITexture::Format::Unknown ) const = 0;
		virtual std::unique_ptr< IBlendState > CreateBlendState( const BlendOperationDesc& rgbOperation, const BlendOperationDesc& alphaDesc ) const = 0;
		virtual std::unique_ptr< renderer::IDepthStencilBuffer > CreateDepthStencilBuffer( Uint32 width, Uint32 height, Bool cubeTexture = false ) const = 0;
		virtual std::unique_ptr< IDepthStencilState > CreateDepthStencilState( DepthStencilComparisonFunc comparisonFunc ) const = 0;
		virtual std::unique_ptr< ISamplerState > CreateSamplerState( SamplerStateFilterType filterType, SamplerStateComparisonType comparisonType ) = 0;
		
		virtual void SetCullingMode( CullingMode mode ) = 0;
		virtual CullingMode GetCullingMode() const = 0;

		virtual void SetFillMode( FillMode mode ) = 0;
		virtual FillMode GetFillMode() const = 0;

		virtual void SetDepthBias( Int32 bias, Float slopeScaledBias, Float clamp ) = 0;
		virtual void SetViewportSize( const Vector2& size ) = 0;
		virtual void SetRenderTargets( const forge::ArraySpan< IRenderTargetView* >& rendererTargetViews, IDepthStencilView* depthStencilView ) = 0;
		virtual void SetSamplerStates( const forge::ArraySpan< ISamplerState* > samplerStates ) = 0;
		virtual void SetShaderResourceViews( const forge::ArraySpan< const IShaderResourceView* >& input, Uint32 startIndex = 0u ) = 0;
		virtual void ClearShaderResourceViews() = 0;

		virtual void OnBeforeDraw() = 0;
		virtual RendererType GetType() const = 0;

		virtual void DrawRawVertices( Uint32 amount ) = 0;
		void Draw( const renderer::Renderable& renderable );
		virtual void Draw( ecs::ArchetypeView archetype, renderer::RenderingPass renderingPass, const renderer::ShaderDefine* shaderDefine = nullptr, forge::ArraySpan< const renderer::IShaderResourceView* > additionalSRVs = {} ) = 0;
		virtual const ecs::Fragment::Type& GetECSFragmentType() const = 0;
		virtual void UpdateRenderableECSArchetype( ecs::MutableArchetypeView archetype, std::function<const renderer::Renderable& ( Uint32 )> renderableGetter ) const = 0;

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

