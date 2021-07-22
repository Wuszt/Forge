#pragma once
#include "../Renderer/IRenderer.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderTargetView.h"
#include "D3D11SwapChain.h"
#include "D3D11VertexShader.h"
#include "D3D11PixelShader.h"
#include "D3D11DepthStencilBuffer.h"

class D3D11Window;
class D3D11Device;
class D3D11Swapchain;
class D3D11VertexShader;
class D3D11PixelShader;
class D3D11InputLayout;
class D3D11VertexBuffer;
class D3D11IndexBuffer;
class IVertexShader;
class IVertexBuffer;
class IInputLayout;
class IIndexBuffer;
class WindowsWindow;
class ICamera;
class D3D11DepthStencilBuffer;
struct ID3D11RasterizerState;

class D3D11Renderer : public IRenderer
{
public:
	D3D11Renderer( const IWindow& window );
	~D3D11Renderer();

	virtual FORGE_INLINE D3D11RenderContext* GetContext() const override
	{
		return m_context.get();
	}

	virtual FORGE_INLINE D3D11RenderTargetView* GetRenderTargetView() const override
	{
		return m_renderTargetView.get();
	}

	virtual FORGE_INLINE D3D11DepthStencilBuffer* GetDepthStencilBuffer() const override
	{
		return m_depthStencilBuffer.get();
	}

	virtual FORGE_INLINE D3D11Swapchain* GetSwapchain() const override
	{
		return m_swapChain.get();
	}

	virtual D3D11VertexShader* GetVertexShader( const std::string& path ) override;
	virtual D3D11PixelShader* GetPixelShader( const std::string& path ) override;

	virtual std::unique_ptr< IInputLayout > CreateInputLayout( const IVertexShader& vertexShader, const IVertexBuffer& vertexBuffer ) const override;
	virtual std::unique_ptr< IVertexBuffer > CreateVertexBuffer( const IVertices& vertices ) const;
	virtual std::unique_ptr< IIndexBuffer > CreateIndexBuffer( const Uint32* indices, Uint32 amount ) const override;

	virtual void SetRenderTargets( std::vector< IRenderTargetView* > rendererTargetViews, IDepthStencilBuffer* depthStencilBuffer ) override;

	virtual void BeginScene() override;

protected:
	virtual std::unique_ptr< IConstantBufferImpl > CreateConstantBufferImpl( void* data, Uint32 dataSize ) const override;

private:
	void SetRenderTargets( std::vector< IRenderTargetView* > rendererTargetViews, D3D11DepthStencilBuffer* depthStencilBuffer );
	void InitializeSwapChainAndContext( const WindowsWindow& window );
	void InitializeViewport( Uint32 width, Uint32 height );
	void InitializeRasterizer();

	std::unique_ptr< D3D11Device > m_device;
	std::unique_ptr< D3D11RenderContext > m_context;
	std::unique_ptr< D3D11Swapchain > m_swapChain;
	std::unique_ptr< D3D11RenderTargetView > m_renderTargetView;

	//todo: create separate class for handling shaders
	std::vector< std::unique_ptr< D3D11VertexShader > > m_vertexShaders;
	std::vector< std::unique_ptr< D3D11PixelShader > > m_pixelShaders;

	std::unique_ptr< D3D11DepthStencilBuffer > m_depthStencilBuffer;

	ID3D11RasterizerState* m_rasterizerState = nullptr;
};

