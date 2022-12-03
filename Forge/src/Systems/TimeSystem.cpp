#include "Fpch.h"
#include "TimeSystem.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
#include "../IMGUI/PublicDefaults.h"
#include "../Core/StringUtils.h"
#endif

IMPLEMENT_TYPE( systems::TimeSystem );

void systems::TimeSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreUpdate, [ this ]() { Update(); } );
	
#ifdef FORGE_IMGUI_ENABLED
	m_overlayHandle = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( [ this ]()
	{
		ImGui::Text( "Time Dil:" );
		ImGui::SameLine();
		if( ImGui::Button( "-" ) )
		{
			m_timeDilation /= 2.0f;
		}

		ImGui::SameLine();
		if( ImGui::Button( forge::String::Printf( "%.3f", m_timeDilation ).c_str() ) )
		{
			m_timeDilation = 1.0f;
		}

		ImGui::SameLine();
		if( ImGui::Button( "+" ) )
		{
			m_timeDilation *= 2.0f;
		}

		ImGui::SameLine();
		if( m_paused )
		{
			if( ImGui::Button( "Resume" ) )
			{
				m_paused = false;
			}
		}
		else
		{
			if( ImGui::Button( "Pause" ) )
			{
				m_paused = true;
			}
		}
	} );
#endif
}

void systems::TimeSystem::Update()
{
#ifdef FORGE_IMGUI_ENABLED
	if( m_paused )
	{
		m_lastDeltaTime = 0.0f;
		return;
	}
#endif

	m_lastDeltaTime = forge::Time::GetDeltaTime() * m_timeDilation;
	m_currentTime += m_lastDeltaTime;
}
