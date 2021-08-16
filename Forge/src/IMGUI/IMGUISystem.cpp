#include "Fpch.h"
#include "IMGUISystem.h"
#include "IMGUIPlatformAdapters.h"
#include "IMGUIRenderAPIAdapters.h"
#include "../Renderer/IRenderer.h"

namespace forge
{
	IMGUISystem::IMGUISystem( IWindow& window, renderer::IRenderer& renderer )
	{
#ifndef FORGE_IMGUI_ENABLED
		FORGE_FATAL( "Imgui is not implemented for this platform" );
#endif

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();

#ifdef FORGE_PLATFORM_WINDOWS
		m_platformAdapter = std::make_unique< windows::IMGUIWindowsAdapter >( window );
#endif

		switch( renderer.GetType() )
		{
		case renderer::RendererType::D3D11:
			m_renderAPIAdapter = std::make_unique< d3d11::IMGUID3D11Adapter >( renderer );
			break;
		default:
			FORGE_FATAL( "Unknown renderer type" );
		}
	}

	IMGUISystem::~IMGUISystem()
	{
		ImGui::DestroyContext();
	}

	void IMGUISystem::OnNewFrame()
	{
		m_renderAPIAdapter->OnNewFrame();
		m_platformAdapter->OnNewFrame();
		ImGui::NewFrame();

		DrawOverlay();
	}

	void IMGUISystem::Render()
	{
		ImGui::Render();
		m_renderAPIAdapter->Render();
	}

	void IMGUISystem::DrawOverlay()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos;
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = work_pos.x + PAD;
		window_pos.y = work_pos.y + PAD;
		window_pos_pivot.x = 0.0f;
		window_pos_pivot.y = 0.0f;
		ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always, window_pos_pivot );
		window_flags |= ImGuiWindowFlags_NoMove;

		ImGui::SetNextWindowBgAlpha( 0.35f );
		Bool tmp = true;
		if( ImGui::Begin( "Overlay", &tmp, window_flags ) )
		{
			m_overlayCallback.Invoke();
		}

		ImGui::End();
	}

}
