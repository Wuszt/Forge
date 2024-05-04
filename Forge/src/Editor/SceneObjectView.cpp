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

	for ( auto* comp : obj->GetComponents() )
	{
		ImGui::CollapsingHeader( comp->GetType().GetName() );
	}
}
