#include "Fpch.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
#include "../IMGUI/PublicDefaults.h"
#include "../IMGUI/IMGUIInstance.h"
#include "../Core/IWindow.h"
#include "../GameEngine/RenderingManager.h"
#include "../Systems/InputSystem.h"
#include "../IMGUI/IMGUIMenuBar.h"

RTTI_IMPLEMENT_TYPE( systems::IMGUISystem );

systems::IMGUISystem::IMGUISystem()
	: m_menuBar( std::make_unique< imgui::MenuBar >( true ) )
{}

systems::IMGUISystem::IMGUISystem( IMGUISystem&& ) = default;

systems::IMGUISystem::~IMGUISystem() = default;

void systems::IMGUISystem::OnPostInit()
{
	auto& inputSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::InputSystem >();
	m_inputHandler = std::make_unique< forge::InputHandler >( inputSystem.RegisterHandler( false, systems::InputSystem::CursorStateRequest::None, systems::InputSystem::HandlerSource::Overlay ) );

	m_imguiInstance = std::make_unique< forge::IMGUIInstance >( GetEngineInstance().GetRenderingManager().GetWindow(), GetEngineInstance().GetRenderingManager().GetRenderer());
	m_preUpdateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::FrameStart, [ & ]()
	{
		m_imguiInstance->OnNewFrame();
	} );

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, [ & ]()
	{
		auto& inputSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::InputSystem >();
		if ( ( m_inputHandler->GetKeyDown( forge::IInput::Key::Control )
			|| m_inputHandler->GetKeyDown( forge::IInput::Key::Shift )
			|| m_inputHandler->GetKeyDown( forge::IInput::Key::Z ) )
			&& m_inputHandler->GetKeys( { forge::IInput::Key::Control, forge::IInput::Key::Shift, forge::IInput::Key::Z } ) )
		{
			m_isInputActive = !m_isInputActive;
			inputSystem.UnregisterHandler( *m_inputHandler );

			if ( m_isInputActive )
			{
				m_inputHandler = std::make_unique< forge::InputHandler >( inputSystem.RegisterHandler( true, systems::InputSystem::CursorStateRequest::Enabled, systems::InputSystem::HandlerSource::Overlay ) );
			}
			else
			{
				m_inputHandler = std::make_unique< forge::InputHandler >( inputSystem.RegisterHandler( false, systems::InputSystem::CursorStateRequest::None, systems::InputSystem::HandlerSource::Overlay ) );
			}
		}

		//DrawOverlay();
		m_menuBar->Draw();
	} );

	m_postRenderingToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostRendering, [ & ]()
	{
		m_imguiInstance->Render();
	} );
}

void systems::IMGUISystem::OnDeinitialize()
{
	auto& inputSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::InputSystem >();
	inputSystem.UnregisterHandler( *m_inputHandler );
}

void systems::IMGUISystem::DrawOverlay()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	const float padding = 10.0f;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 workPos = viewport->WorkPos;
	ImVec2 workSize = viewport->WorkSize;
	ImVec2 windowPos;
	ImVec2 windowPosPivot;
	windowPos.x = windowPos.x + padding;
	windowPos.y = windowPos.y + padding;
	windowPosPivot.x = 0.0f;
	windowPosPivot.y = 0.0f;
	ImGui::SetNextWindowPos( windowPos, ImGuiCond_Always, windowPosPivot );
	windowFlags |= ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowBgAlpha( 0.35f );
	Bool tmp = true;
	if( ImGui::Begin( "Overlay", &tmp, windowFlags ) )
	{
		m_overlayCallback.Invoke();
	}

	ImGui::End();
}
#endif