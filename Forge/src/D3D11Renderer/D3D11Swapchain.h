#pragma once
#include "../Renderer/ISwapchain.h"

struct IDXGISwapChain;
struct DXGI_SWAP_CHAIN_DESC;

struct HWND__;
typedef HWND__* HWND;

namespace renderer
{
	class ITexture;
}

namespace d3d11
{
	class SwapchainTexture;

	class D3D11Swapchain : public renderer::ISwapchain
	{
	public:
		static DXGI_SWAP_CHAIN_DESC GetSwapChainDescription( Uint32 width, Uint32 height, HWND hwnd );

		D3D11Swapchain( const D3D11Device& device, const D3D11RenderContext& context, IDXGISwapChain* swapChain );
		~D3D11Swapchain();

		IDXGISwapChain* GetSwapChain() const
		{
			return m_swapChain;
		}

		virtual void Present() override;
		virtual renderer::ITexture& GetBackBuffer() const override;

		virtual void Resize( Uint32 width, Uint32 height ) override;

	private:
		IDXGISwapChain* m_swapChain = nullptr;
		const D3D11Device& m_device;
		const D3D11RenderContext& m_context;
		std::unique_ptr< SwapchainTexture > m_backBuffer;
	};
}
