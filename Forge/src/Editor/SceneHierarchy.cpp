#include "Fpch.h"
#include "SceneHierarchy.h"
#include "../GameEngine/Object.h"
#include "../../External/imgui/imgui.h"
#include "SceneEditor.h"
#include "../Systems/TransformComponent.h"
#include "EditorTags.h"
#include "../GameEngine/SceneManager.h"
#include "../GameEngine/Scene.h"

editor::SceneHierarchy::SceneHierarchy( editor::SceneEditor& sceneEditor )
	: WindowBase( sceneEditor.GetEngineInstance(), &sceneEditor, false )
{}

editor::SceneEditor& editor::SceneHierarchy::GetSceneEditor()
{
	return static_cast< editor::SceneEditor& >( *GetParent() );
}

void editor::SceneHierarchy::Draw()
{
	GetEngineInstance().GetSceneManager().GetScene().VisitSceneObjects( [ & ]( forge::ObjectID objectID )
	{
		DrawObjectEntry( objectID );
	} );
}

template< class TFunc >
static void VisitHierarchyBranch( forge::EngineInstance& engineInstance, forge::ObjectID root, const TFunc& func )
{
	forge::Object* obj = engineInstance.GetObjectsManager().GetObject( root );
	if ( forge::TransformComponent* transformComp = obj->GetComponent< forge::TransformComponent >() )
	{
		for ( auto childID : transformComp->GetChildren() )
		{
			func( childID );
			VisitHierarchyBranch( engineInstance, childID, func );
		}
	}
}

void editor::SceneHierarchy::DrawObjectEntry( forge::ObjectID objectID )
{
	forge::Object* obj = GetEngineInstance().GetObjectsManager().GetObject( objectID );

	const auto entityID = GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( objectID );
	auto& ecsManager = GetEngineInstance().GetECSManager();

	std::vector< forge::ObjectID > children;
	if ( forge::TransformComponent* transformComp = obj->GetComponent< forge::TransformComponent >() )
	{
		children = transformComp->GetChildren();
	}

	if ( children.empty() )
	{
		if ( ImGui::Selectable( obj->GetName(), objectID == GetSceneEditor().GetSelectedObject() ) )
		{
			GetSceneEditor().SelectObject( objectID );
		}
		if ( ImGui::BeginPopupContextItem() )
		{
			GetSceneEditor().SelectObject( objectID );
			if ( ImGui::MenuItem( "Delete" ) )
			{
				GetSceneEditor().SelectObject( {} );
				GetEngineInstance().GetObjectsManager().RequestDestructingObject( objectID );
			}
			ImGui::EndPopup();
		}
	}
	else
	{
		if ( ImGui::TreeNodeEx( obj->GetName(), ImGuiTreeNodeFlags_OpenOnArrow | ( objectID == GetSceneEditor().GetSelectedObject() ? ImGuiTreeNodeFlags_Selected : 0 ) ) )
		{
			if ( ImGui::IsItemClicked() )
			{
				GetSceneEditor().SelectObject( objectID );
			}

			for ( auto childID : children )
			{
				DrawObjectEntry( childID );
			}

			ImGui::TreePop();
		}
		else
		{
			if ( ImGui::IsItemClicked() )
			{
				GetSceneEditor().SelectObject( objectID );
			}
		}
	}
}
