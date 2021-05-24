#pragma once

class D3D11Window;
class D3D11Device;
class D3D11Context;
class D3D11Swapchain;
class D3D11RenderTargetView;
class D3D11VertexShader;
class D3D11PixelShader;

class D3D11Renderer
{
public:
	D3D11Renderer( Uint32 width, Uint32 height );
	~D3D11Renderer();

	FORGE_INLINE D3D11Context* GetContext() const
	{
		return m_context.get();
	}

	D3D11VertexShader* GetVertexShader( const std::string& path );
	D3D11PixelShader* GetPixelShader( const std::string& path );


private:
	void InitializeSwapChainAndContext( Uint32 width, Uint32 height, const D3D11Window& window );

	std::unique_ptr< D3D11Window > m_window;
	std::unique_ptr< D3D11Device > m_device;
	std::unique_ptr< D3D11Context > m_context;
	std::unique_ptr< D3D11Swapchain > m_swapChain;
	std::unique_ptr< D3D11RenderTargetView > m_renderTargetView;

	//todo: create separate class for handling shaders
	std::vector< std::unique_ptr< D3D11VertexShader > > m_vertexShaders;
	std::vector< std::unique_ptr< D3D11PixelShader > > m_pixelShaders;
};

