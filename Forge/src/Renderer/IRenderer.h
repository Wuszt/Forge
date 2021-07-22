#pragma once
#include "ConstantBuffer.h"

class IRenderContext;
class IRenderTargetView;
class ISwapchain;
class IVertexShader;
class IPixelShader;
class IVertexBuffer;
class IInputLayout;
class IIndexBuffer;
class IWindow;
class ICamera;
struct IVertices;
class IConstantBufferImpl;
class IDepthStencilBuffer;

class IRenderer
{
public:
	static std::unique_ptr< IRenderer > CreateRenderer( const IWindow& window );

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

protected:
	virtual std::unique_ptr< IConstantBufferImpl > CreateConstantBufferImpl( void* data, Uint32 dataSize ) const = 0;

public:
	template< class T >
	std::unique_ptr< ConstantBuffer< T > > GetConstantBuffer() const
	{
		auto constBuffer = std::make_unique< ConstantBuffer< T > >();
		auto& data = constBuffer->GetData();
		constBuffer->SetImpl( std::move( CreateConstantBufferImpl( &data, sizeof( data ) ) ) );
		return constBuffer;
	}
};

