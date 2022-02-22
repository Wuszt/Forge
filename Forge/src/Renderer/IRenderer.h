#pragma once
#include "ConstantBuffer.h"
#include "ITexture.h"

namespace forge
{
	class IWindow;
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
	class ResourcesManager;
	class ITexturesLoader;
	class ISamplerState;
	class IBlendState;
	enum class SamplerStateFilterType;
	struct BlendOperationDesc;

	enum class RendererType
	{
		D3D11,
		Unknown
	};

	struct IRawRenderablesPack
	{
		virtual ~IRawRenderablesPack() = default;
	};

	class IRenderer
	{
	public:
		static std::unique_ptr< IRenderer > CreateRenderer( forge::IWindow& window, RendererType type );
		IRenderer();
		virtual ~IRenderer();

		virtual IRenderContext* GetContext() const = 0;
		virtual IDepthStencilBuffer* GetDepthStencilBuffer() const = 0;
		virtual ISwapchain* GetSwapchain() const = 0;
		virtual IShadersManager* GetShadersManager() const = 0;

		virtual std::unique_ptr< IInputLayout > CreateInputLayout( const IVertexShader& vertexShader, const IVertexBuffer& vertexBuffer ) const = 0;
		virtual std::unique_ptr< IVertexBuffer > CreateVertexBuffer( const Vertices& vertices ) const = 0;
		virtual std::unique_ptr< IIndexBuffer > CreateIndexBuffer( const Uint32* indices, Uint32 amount ) const = 0;
		virtual std::unique_ptr< ITexture > CreateTexture( Uint32 width, Uint32 height, ITexture::Flags flags, ITexture::Format format, ITexture::Format srvFormat = ITexture::Format::Unknown ) const = 0;
		virtual std::unique_ptr< IBlendState > CreateBlendState( const renderer::BlendOperationDesc& rgbOperation, const renderer::BlendOperationDesc& alphaDesc ) = 0;

		void Initialize();

		virtual void SetRenderTargets( const forge::ArraySpan< IRenderTargetView* const >& rendererTargetViews, IDepthStencilBuffer* depthStencilBuffer ) = 0;

		virtual std::unique_ptr< ISamplerState > CreateSamplerState( SamplerStateFilterType filterType ) = 0;
		virtual void SetSamplerStates( const forge::ArraySpan< ISamplerState* > samplerStates ) = 0;

		virtual void SetShaderResourceViews( const forge::ArraySpan< IShaderResourceView* >& input ) = 0;
		virtual void ClearShaderResourceViews() = 0;

		virtual void OnBeforeDraw() = 0;

		virtual RendererType GetType() const = 0;

		virtual void DrawRawVertices( Uint32 amount ) = 0;
		void Draw( const renderer::Renderable& renderable );
		virtual void Draw( const renderer::IRawRenderablesPack& rawRenderables ) = 0;

		virtual std::unique_ptr< IRawRenderablesPack > CreateRawRenderablesPackage( const forge::ArraySpan< const Renderable* const >& renderables ) const = 0;

		template< class T >
		FORGE_INLINE std::unique_ptr< StaticConstantBuffer< T > > CreateStaticConstantBuffer() const
		{
			auto constBuffer = std::make_unique< StaticConstantBuffer< T > >();
			constBuffer->SetImpl( CreateConstantBufferImpl() );
			return constBuffer;
		}

		FORGE_INLINE std::unique_ptr< ConstantBuffer > CreateConstantBuffer() const
		{
			auto constBuffer = std::make_unique< ConstantBuffer >();
			constBuffer->SetImpl( CreateConstantBufferImpl() );
			return constBuffer;
		}

		FORGE_INLINE std::unique_ptr< ConstantBuffer > CreateConstantBufferFromOther( const ConstantBuffer& data ) const
		{
			auto buff = CreateConstantBuffer();
			buff->CopyDataFrom( data );
			buff->UpdateBuffer();
			return buff;
		}

		virtual std::unique_ptr< IConstantBufferImpl > CreateConstantBufferImpl() const = 0;

		FORGE_INLINE ResourcesManager& GetResourceManager() const
		{
			return *m_resourcesManager;
		}

		virtual std::unique_ptr< ITexturesLoader > CreateTexturesLoader() const = 0;
		virtual Vector2 GetResolution() const = 0;

	private:
		std::unique_ptr< ResourcesManager > m_resourcesManager;
	};
}

