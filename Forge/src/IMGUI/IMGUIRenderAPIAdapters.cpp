#include "Fpch.h"
#include "IMGUIRenderAPIAdapters.h"

#include "backends/imgui_impl_dx11.h"
#include "../D3D11Renderer/D3D11Renderer.h"
#include "../D3D11Renderer/D3D11RenderContext.h"
#include "../D3D11Renderer/D3D11Device.h"

namespace d3d11
{
	IMGUID3D11Adapter::IMGUID3D11Adapter( renderer::Renderer& renderer )
		: m_renderer( renderer )
	{
		FORGE_ASSERT( dynamic_cast<const D3D11Renderer*>( &renderer ) );
		const D3D11Renderer& d3d11Renderer = static_cast<const D3D11Renderer&>( renderer );
		ImGui_ImplDX11_Init( d3d11Renderer.GetDevice()->GetDevice(), d3d11Renderer.GetContext()->GetDeviceContext() );
	}

	void IMGUID3D11Adapter::Render()
	{
		renderer::IRenderTargetView* views[] = { m_renderer.GetSwapchain()->GetBackBuffer().GetRenderTargetView() };
		m_renderer.SetRenderTargets( views, nullptr );

		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );

		if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void IMGUID3D11Adapter::OnNewFrame()
	{
		ImGui_ImplDX11_NewFrame();
	}

	IMGUID3D11Adapter::~IMGUID3D11Adapter()
	{
		ImGui_ImplDX11_Shutdown();
	}
}