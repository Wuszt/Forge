#include "Fpch.h"
#include "WindowBase.h"
#include "../../External/imgui/imgui.h"
#include "../IMGUI/IMGUIHelpers.h"
#include "../IMGUI/IMGUIMenuBar.h"

editor::WindowBase::WindowBase( forge::EngineInstance& engineInstance, WindowBase* parent, Bool withMenuBar )
	: m_engineInstance( engineInstance )
	, m_parent( parent )
{
	if ( withMenuBar )
	{
		m_menuBar = std::make_unique< imgui::MenuBar >();
	}
}

editor::WindowBase::~WindowBase() = default;

void editor::WindowBase::Update()
{
	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f } );
	ImGui::Begin( GetName(), nullptr, m_menuBar ? ImGuiWindowFlags_MenuBar : 0 );

	m_currentSize = forge::imgui::CastToForge( ImGui::GetWindowContentRegionMax() ) - forge::imgui::CastToForge( ImGui::GetWindowContentRegionMin() );

	if ( m_menuBar )
	{
		m_menuBar->Draw();
	}

	Draw();

	for ( auto& child : m_children )
	{
		child->Update();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

Vector2 editor::WindowBase::GetSize() const
{
	return m_currentSize;
}
