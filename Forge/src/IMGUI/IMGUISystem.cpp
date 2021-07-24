#include "Fpch.h"
#include "IMGUISystem.h"
#include "IMGUIPlatformAdapters.h"
#include "IMGUIRenderAPIAdapters.h"
#include "../Renderer/IRenderer.h"

IMGUISystem::IMGUISystem( IWindow& window, IRenderer& renderer )
{
#ifndef FORGE_IMGUI_ENABLED
FORGE_FATAL( "Imgui is not implemented for this platform" );
#endif

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui::StyleColorsDark();

#ifdef FORGE_PLATFORM_WINDOWS
	m_platformAdapter = std::make_unique< IMGUIWindowsAdapter >( window );
#endif

	switch( renderer.GetType() )
	{
	case RendererType::D3D11:
		m_renderAPIAdapter = std::make_unique< IMGUID3D11Adapter >( renderer );
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
}

void IMGUISystem::Render()
{
	ImGui::Render();
	m_renderAPIAdapter->Render();
}
