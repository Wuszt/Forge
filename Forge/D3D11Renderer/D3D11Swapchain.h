#pragma once

class D3D11Texture;
struct IDXGISwapChain;

class D3D11Swapchain
{
public:
	static DXGI_SWAP_CHAIN_DESC GetSwapChainDescription( Uint32 width, Uint32 height, HWND hwnd );

	D3D11Swapchain( IDXGISwapChain* swapChain );
	~D3D11Swapchain();

	FORGE_INLINE IDXGISwapChain* const& GetSwapChain() const
	{
		return m_swapChain;
	}

	void Present();
	std::unique_ptr< D3D11Texture > GetBackBuffer() const;

private:
	IDXGISwapChain* m_swapChain = nullptr;
};

