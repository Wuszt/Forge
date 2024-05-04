#include "Fpch.h"
#include "SceneObjectView.h"
#include "SceneEditor.h"
#include "../GameEngine/ObjectsManager.h"
#include "../../External/imgui/imgui.h"
#include "../IMGUI/IMGUIHelpers.h"

editor::SceneObjectView::SceneObjectView( SceneEditor& sceneEditor )
	: WindowBase( sceneEditor.GetEngineInstance(), &sceneEditor, false )
{}

editor::SceneEditor& editor::SceneObjectView::GetSceneEditor()
{
	return static_cast< editor::SceneEditor& >( *GetParent() );
}

template< class TFunc >
static void DrawProperty( const char* name, const TFunc& valueWidget )
{
	ImGui::TableNextColumn();
	ImGui::AlignTextToFramePadding();
	ImGui::Text( name );
	ImGui::TableNextColumn();
	ImGui::AlignTextToFramePadding();
	valueWidget();
	ImGui::TableNextRow();
}

void editor::SceneObjectView::Draw()
{
	auto objectID = GetSceneEditor().GetSelectedObject();
	if ( !objectID.IsValid() )
	{
		return;
	}

	auto* obj = GetEngineInstance().GetObjectsManager().GetObject( objectID );

	ImGui::SeparatorText( "Object" );
	// Object
	if ( ImGui::BeginTable( "Object Properties", 2 ) )
	{
		DrawProperty( "Type", [ & ](){ ImGui::Text( obj->GetType().GetName() ); } );
		DrawProperty( "Name", [ & ]()
			{
				std::string name = obj->GetName();
				if ( forge::imgui::InputText( "##Name", name ) )
				{
					obj->SetName( name );
				}
			} );
	}
	ImGui::EndTable();

	// Components
	ImGui::SeparatorText( "Components" );

	if ( ImGui::BeginPopupContextItem( "AddComponent" ) )
	{
		ImGui::SeparatorText( "Select component type to add" );
		rtti::Get().VisitTypes( [ & ]( const rtti::Type& type )
			{
				if ( type.InheritsFrom< forge::IComponent >() && !type.IsAbstract() )
				{
					const forge::IComponent::Type& componentType = static_cast< const forge::IComponent::Type& >( type );
					if ( !obj->HasComponent( componentType ) )
					{
						if ( ImGui::Button( type.GetName() ) )
						{
							obj->AddComponent( componentType );
							ImGui::CloseCurrentPopup();
						}
					}
				}
			} );
		ImGui::EndPopup();
	}

	if ( ImGui::Button( "Add new component" ) )
	{
		ImGui::OpenPopup( "AddComponent" );
	}

	ImGui::NewLine();
	for ( auto* comp : obj->GetComponents() )
	{
		const Float width = ImGui::GetColumnWidth();
		ImGui::CollapsingHeader( comp->GetType().GetName(), ImGuiTreeNodeFlags_AllowOverlap );
		ImGui::SameLine();
		constexpr Float buttonWidth = 75.0f;
		constexpr Float marginFromRight = 10.0f;
		ImGui::SetCursorPosX( width - buttonWidth - marginFromRight );
		if ( ImGui::Button( forge::String::Printf( "Remove##%s", comp->GetType().GetName() ).c_str(), { buttonWidth, 0.0f } ) )
		{
			obj->RemoveComponent( comp->GetType() );
		}
	}
}
