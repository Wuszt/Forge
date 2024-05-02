#include "Fpch.h"
#include "PanelBase.h"
#include "../../External/imgui/imgui.h"
#include "../IMGUI/IMGUIHelpers.h"
#include "../IMGUI/IMGUIMenuBar.h"

editor::PanelBase::PanelBase( Bool withMenuBar, forge::EngineInstance& engineIntance )
	: m_engineInstance( engineIntance )
{
	if ( withMenuBar )
	{
		m_menuBar = std::make_unique< imgui::MenuBar >();
	}
}

editor::PanelBase::~PanelBase() = default;

void editor::PanelBase::Update()
{
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f } );
	ImGui::Begin( GetName(), nullptr, m_menuBar ? ImGuiWindowFlags_MenuBar : 0 );

	m_currentSize = forge::imgui::CastToForge( ImGui::GetWindowContentRegionMax() ) - forge::imgui::CastToForge( ImGui::GetWindowContentRegionMin() );

	if ( m_menuBar )
	{
		m_menuBar->Draw();
	}

	Draw();

	ImGui::End();
	ImGui::PopStyleVar();
}

Vector2 editor::PanelBase::GetSize() const
{
	return m_currentSize;
}
