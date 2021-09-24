#include "Fpch.h"
#include "DebugSystem.h"
#include "IMGUISystem.h"
#include "../Core/FPSCounter.h"
#include "../../External/imgui/imgui.h"

void systems::DebugSystem::OnInitialize()
{
	m_fpsCounterDrawingToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( []()
	{
		Float fps = forge::FPSCounter::GetAverageFPS( 1u );
		Vector4 color{ 1.0f, 0.0f, 0.0f, 1.0f };

		if( fps > 30.0f )
		{
			if( fps > 6.0f )
			{
				color = { 0.0f, 1.0f, 0.0f, 1.0f };
			}
			else
			{
				color = { 1.0f, 1.0f, 0.0f, 1.0f };
			}
		}

		Uint32 bufferSize = 0u;
		Uint32 currentOffset = 0u;
		Float* buff = forge::FPSCounter::GetFPSCounter().GetBuffer( currentOffset, bufferSize );

		ImGui::PlotHistogram( "", buff, bufferSize, currentOffset, NULL, 0.0f, 120.0f, ImVec2( 100.0f, 40.0f ) );
		ImGui::SameLine();

		ImGui::SetWindowFontScale( 1.5f );
		ImGui::PushStyleColor( ImGuiCol_Text, { color.X, color.Y, color.Z, color.W } );
		ImGui::Text( "FPS: %.2f", fps );
		ImGui::PopStyleColor();
		ImGui::SetWindowFontScale( 1.0f );
	} ) );
}
