#include "Fpch.h"
#include "CamerasSystem.h"
#include "CameraComponent.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
#include "../../External/imgui/imgui.h"
#endif

void systems::CamerasSystem::OnInitialize()
{
	m_updateToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, std::bind( &systems::CamerasSystem::Update, this ) ) );

#ifdef FORGE_IMGUI_ENABLED
	m_debugOverlayToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( [ & ]()
	{
		if( GetActiveCamera() )
		{
			ImGui::Text( "Cam Pos:%s", GetActiveCamera()->GetCamera().GetPosition().ToDebugString().c_str() );
			ImGui::Text( "Cam Fwd:%s", ( GetActiveCamera()->GetCamera().GetOrientation() * Vector3::EY() ).ToDebugString( 3 ).c_str() );
		}
	} ) );
#endif
}

void systems::CamerasSystem::Update()
{
	if( m_activeCamera )
	{
		m_activeCamera->Update();
	}
}