#pragma once
class D3D11Device;

class D3D11RenderTargetView
{
public:
	D3D11RenderTargetView( const D3D11Device& device, const D3D11Swapchain& swapChain );
	~D3D11RenderTargetView();

	FORGE_INLINE ID3D11RenderTargetView * const& GetRenderTargetView() const
	{
		return m_renderTargetView;
	}

private:
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
};

