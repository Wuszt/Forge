#include "Fpch.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
#include "../IMGUI/PublicDefaults.h"
#include "../IMGUI/IMGUIInstance.h"
#include "../Core/IWindow.h"

IMPLEMENT_TYPE( systems, IMGUISystem );

systems::IMGUISystem::IMGUISystem() = default;
systems::IMGUISystem::~IMGUISystem() = default;

void systems::IMGUISystem::OnInitialize()
{
	m_imguiInstance = std::make_unique< forge::IMGUIInstance >( GetEngineInstance().GetWindow(), GetEngineInstance().GetRenderer() );
	m_preUpdateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreUpdate, [ & ]()
	{
		m_imguiInstance->OnNewFrame();
	} );

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, [ & ]()
	{
		DrawOverlay();
		DrawTopBar();
	} );

	m_postRenderingToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostRendering, [ & ]()
	{
		m_imguiInstance->Render();
	} );
}

void systems::IMGUISystem::DrawOverlay()
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

void systems::IMGUISystem::DrawTopBar()
{
	if( ImGui::BeginMainMenuBar() )
	{
		if( ImGui::BeginMenu( "Systems Debug" ) )
		{
			for( auto& systemToDebug : m_systemsToDebug )
			{
				if( systemToDebug.m_system.HasDebug() )
				{
					ImGui::MenuItem( systemToDebug.m_system.GetType().GetName( false ), nullptr, systemToDebug.m_enabled );
				}
			}

			ImGui::EndMenu();
		}

		if( ImGui::BeginMenu( "Other" ) )
		{
			ImGui::MenuItem( "Show IMGUI Demo", nullptr, &m_imguiDemoEnabled );
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if( m_imguiDemoEnabled )
	{
		ImGui::ShowDemoWindow();
	}
}

#endif