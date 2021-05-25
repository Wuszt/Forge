#pragma once

class D3D11Window;
class D3D11Device;
class D3D11Context;
class D3D11Swapchain;
class D3D11RenderTargetView;
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11InputLayout;
class D3D11VertexBuffer;
class D3D11IndexBuffer;
struct Vertex;

class D3D11Renderer
{
public:
	D3D11Renderer( Uint32 width, Uint32 height );
	~D3D11Renderer();

	FORGE_INLINE D3D11Context* GetContext() const
	{
		return m_context.get();
	}

	FORGE_INLINE D3D11RenderTargetView* GetRenderTargetView() const
	{
		return m_renderTargetView.get();
	}

	FORGE_INLINE D3D11Swapchain* GetSwapchain() const
	{
		return m_swapChain.get();
	}

	D3D11VertexShader* GetVertexShader( const std::string& path );
	D3D11PixelShader* GetPixelShader( const std::string& path );

	std::unique_ptr< D3D11InputLayout > GetInputLayout( const D3D11VertexShader& vertexShader, const D3D11VertexBuffer& vertexBuffer ) const;
	std::unique_ptr< D3D11VertexBuffer > GetVertexBuffer( const Vertex* vertices, Uint32 amount ) const;
	std::unique_ptr< D3D11IndexBuffer > GetIndexBuffer( const Uint32* indices, Uint32 amount ) const;

private:
	void InitializeSwapChainAndContext( Uint32 width, Uint32 height, const D3D11Window& window );
	void InitializeViewport( Uint32 width, Uint32 height );

	std::unique_ptr< D3D11Window > m_window;
	std::unique_ptr< D3D11Device > m_device;
	std::unique_ptr< D3D11Context > m_context;
	std::unique_ptr< D3D11Swapchain > m_swapChain;
	std::unique_ptr< D3D11RenderTargetView > m_renderTargetView;

	//todo: create separate class for handling shaders
	std::vector< std::unique_ptr< D3D11VertexShader > > m_vertexShaders;
	std::vector< std::unique_ptr< D3D11PixelShader > > m_pixelShaders;
};

