#pragma once
class D3D11Device;
class D3D11Swapchain;
struct Vector4;
struct ID3D11RenderTargetView;

class D3D11RenderTargetView
{
public:
	D3D11RenderTargetView( D3D11Context* contextPtr, const D3D11Device& device, const D3D11Swapchain& swapChain );
	~D3D11RenderTargetView();

	FORGE_INLINE ID3D11RenderTargetView * const& GetRenderTargetView() const
	{
		return m_renderTargetView;
	}

	void Set();
	void Clear( const Vector4& bgColor = { 0.0f, 0.0f, 0.0f, 0.0f } );

private:
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	D3D11Context* m_contextPtr = nullptr;
};

