#pragma once
#include "ConstantBuffer.h"

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
	struct IVertices;
	class IConstantBufferImpl;
	class IDepthStencilBuffer;

	enum class RendererType
	{
		D3D11,
		Unknown
	};

	class IRenderer
	{
	public:
		static std::unique_ptr< IRenderer > CreateRenderer( forge::IWindow& window, RendererType type );

		virtual ~IRenderer() = default;

		virtual IRenderContext* GetContext() const = 0;
		virtual IRenderTargetView* GetRenderTargetView() const = 0;
		virtual IDepthStencilBuffer* GetDepthStencilBuffer() const = 0;
		virtual ISwapchain* GetSwapchain() const = 0;

		virtual IVertexShader* GetVertexShader( const std::string& path ) = 0;
		virtual IPixelShader* GetPixelShader( const std::string& path ) = 0;

		virtual std::unique_ptr< IInputLayout > CreateInputLayout( const IVertexShader& vertexShader, const IVertexBuffer& vertexBuffer ) const = 0;
		virtual std::unique_ptr< IVertexBuffer > CreateVertexBuffer( const IVertices& vertices ) const = 0;
		virtual std::unique_ptr< IIndexBuffer > CreateIndexBuffer( const Uint32* indices, Uint32 amount ) const = 0;

		virtual void SetRenderTargets( std::vector< IRenderTargetView* > rendererTargetViews, IDepthStencilBuffer* depthStencilBuffer ) = 0;

		virtual void BeginScene() = 0;

		virtual RendererType GetType() const = 0;

	protected:
		virtual std::unique_ptr< IConstantBufferImpl > CreateConstantBufferImpl() const = 0;

	public:
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
	};
}

