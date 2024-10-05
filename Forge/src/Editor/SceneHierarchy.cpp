#include "Fpch.h"
#include "SceneHierarchy.h"
#include "../ECS/Query.h"
#include "../GameEngine/Object.h"
#include "../../External/imgui/imgui.h"
#include "SceneEditor.h"
#include "../Systems/TransformComponent.h"
#include "EditorTags.h"

editor::SceneHierarchy::SceneHierarchy( editor::SceneEditor& sceneEditor )
	: WindowBase( sceneEditor.GetEngineInstance(), &sceneEditor, false )
{}

editor::SceneEditor& editor::SceneHierarchy::GetSceneEditor()
{
	return static_cast< editor::SceneEditor& >( *GetParent() );
}

void editor::SceneHierarchy::Draw()
{
	ecs::Query objectsQuery( GetEngineInstance().GetECSManager() );
	objectsQuery.AddFragmentRequirement< forge::ObjectFragment >( ecs::Query::RequirementType::Included );

	std::unordered_set< forge::ObjectID > drawnIds;

	objectsQuery.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype )
		{
			auto objectFragments = archetype.GetFragments< forge::ObjectFragment >();
			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				auto objectFragment = objectFragments[ i ];
				DrawObjectEntry( objectFragment.m_objectID, drawnIds );
			}
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

void editor::SceneHierarchy::DrawObjectEntry( forge::ObjectID objectID, std::unordered_set< forge::ObjectID >& drawnObjects )
{
	if ( drawnObjects.contains( objectID ) )
	{
		return;
	}

	drawnObjects.emplace( objectID );

	forge::Object* obj = GetEngineInstance().GetObjectsManager().GetObject( objectID );

	const auto entityID = GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( objectID );
	auto& ecsManager = GetEngineInstance().GetECSManager();
	const Bool isEditorOnlyObject = ecsManager.GetEntityArchetypeId( entityID ).ContainsTag< editor::EditorObjectTag >();

	if ( obj->GetType().HasMetadata( "EditorOnly" ) || isEditorOnlyObject )
	{
		return;
	}

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
				DrawObjectEntry( childID, drawnObjects );
			}

			ImGui::TreePop();
		}
		else
		{
			if ( ImGui::IsItemClicked() )
			{
				GetSceneEditor().SelectObject( objectID );
			}

			VisitHierarchyBranch( GetEngineInstance(), objectID, [ & ]( forge::ObjectID object )
				{
					drawnObjects.emplace( object );
				} );
		}
	}
}
