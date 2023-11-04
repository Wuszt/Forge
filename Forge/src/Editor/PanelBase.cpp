#include "Fpch.h"
#include "PanelBase.h"
#include "../../External/imgui/imgui.h"
#include "../IMGUI/IMGUIHelpers.h"

editor::PanelBase::PanelBase( forge::EngineInstance& engineIntance )
	: m_engineInstance( engineIntance )
{}

void editor::PanelBase::Update()
{
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f } );
	ImGui::Begin( GetName() );

	m_currentSize = forge::imgui::CastToForge( ImGui::GetWindowContentRegionMax() ) - forge::imgui::CastToForge( ImGui::GetWindowContentRegionMin() );

	Draw();

	ImGui::End();
	ImGui::PopStyleVar();
}

Vector2 editor::PanelBase::GetSize() const
{
	return m_currentSize;
}
