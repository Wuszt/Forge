#pragma once
#include "../Renderer/ISwapchain.h"

class D3D11Texture;
struct IDXGISwapChain;
struct DXGI_SWAP_CHAIN_DESC;

struct HWND__;
typedef HWND__* HWND;

class D3D11Swapchain : public ISwapchain
{
public:
	static DXGI_SWAP_CHAIN_DESC GetSwapChainDescription( Uint32 width, Uint32 height, HWND hwnd );

	D3D11Swapchain( IDXGISwapChain* swapChain );
	~D3D11Swapchain();

	FORGE_INLINE IDXGISwapChain* const& GetSwapChain() const
	{
		return m_swapChain;
	}

	virtual void Present() override;
	std::unique_ptr< D3D11Texture > GetBackBuffer() const;

	virtual void Resize( Uint32 width, Uint32 height ) override;

private:
	IDXGISwapChain* m_swapChain = nullptr;
};

