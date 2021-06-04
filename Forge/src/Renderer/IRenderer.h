#pragma once

class IRenderContext;
class IRenderTargetView;
class ISwapchain;
class IVertexShader;
class IPixelShader;
class IVertexBuffer;
class IInputLayout;
class IIndexBuffer;
class IWindow;
struct IVertices;

class IRenderer
{
public:
	static std::unique_ptr< IRenderer > CreateRenderer( const IWindow& window );

	virtual ~IRenderer() = default;

	virtual IRenderContext* GetContext() const = 0;
	virtual IRenderTargetView* GetRenderTargetView() const = 0;
	virtual ISwapchain* GetSwapchain() const = 0;

	virtual IVertexShader* GetVertexShader( const std::string& path ) = 0;
	virtual IPixelShader* GetPixelShader( const std::string& path ) = 0;

	virtual std::unique_ptr< IInputLayout > GetInputLayout( const IVertexShader& vertexShader, const IVertexBuffer& vertexBuffer ) const = 0;
	virtual std::unique_ptr< IVertexBuffer > GetVertexBuffer( const IVertices& vertices ) const = 0;
	virtual std::unique_ptr< IIndexBuffer > GetIndexBuffer( const Uint32* indices, Uint32 amount ) const = 0;
};

