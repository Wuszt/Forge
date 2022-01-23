#pragma once
#include "../Renderer/ISwapchain.h"

struct IDXGISwapChain;
struct DXGI_SWAP_CHAIN_DESC;

struct HWND__;
typedef HWND__* HWND;

namespace d3d11
{
	class D3D11Texture;

	class D3D11Swapchain : public renderer::ISwapchain
	{
	public:
		static DXGI_SWAP_CHAIN_DESC GetSwapChainDescription( Uint32 width, Uint32 height, HWND hwnd );

		D3D11Swapchain( const D3D11Device& device, IDXGISwapChain* swapChain );
		~D3D11Swapchain();

		FORGE_INLINE IDXGISwapChain* GetSwapChain() const
		{
			return m_swapChain;
		}

		virtual void Present() override;
		std::unique_ptr< D3D11Texture > GetBackBuffer() const;

		virtual void Resize( Uint32 width, Uint32 height ) override;

	private:
		IDXGISwapChain* m_swapChain = nullptr;
		const D3D11Device& m_device;
	};
}
