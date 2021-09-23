#include "Fpch.h"
#include "IMGUIInstance.h"
#include "IMGUIPlatformAdapters.h"
#include "IMGUIRenderAPIAdapters.h"
#include "../Renderer/IRenderer.h"

namespace forge
{
	IMGUIInstance::IMGUIInstance( IWindow& window, renderer::IRenderer& renderer )
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

	IMGUIInstance::~IMGUIInstance()
	{
		ImGui::DestroyContext();
	}

	void IMGUIInstance::OnNewFrame()
	{
		m_renderAPIAdapter->OnNewFrame();
		m_platformAdapter->OnNewFrame();
		ImGui::NewFrame();
	}

	void IMGUIInstance::Render()
	{
		ImGui::Render();
		m_renderAPIAdapter->Render();
	}

}
