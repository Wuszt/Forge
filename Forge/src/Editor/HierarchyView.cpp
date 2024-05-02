#include "Fpch.h"
#include "HierarchyView.h"
#include "../ECS/Query.h"
#include "../GameEngine/Object.h"
#include "../../External/imgui/imgui.h"

void editor::HierarchyView::Draw()
{
	ecs::Query objectsQuery( GetEngineInstance().GetECSManager() );
	objectsQuery.AddFragmentRequirement< forge::ObjectFragment >( ecs::Query::RequirementType::Included );

	objectsQuery.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype )
		{
			auto objectFragments = archetype.GetFragments< forge::ObjectFragment >();
			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				auto objectFragment = objectFragments[ i ];
				forge::Object* obj = GetEngineInstance().GetObjectsManager().GetObject( objectFragment.m_objectID );
				
				const char* typeName = obj->GetType().GetName();
				const char editorNamespace[] = "editor::";
				constexpr Uint32 editorNamespaceLength = sizeof( editorNamespace ) - 1;

				if ( std::strncmp( typeName, editorNamespace, editorNamespaceLength ) != 0 )
				{
					const char* name = obj->GetName();
					if ( ImGui::Selectable( forge::String::Printf( "[%s] %s", typeName, name ).c_str(), objectFragment.m_objectID == m_getSelectedObjectFunc() ) )
					{
						m_selectObjectFunc( objectFragment.m_objectID );
					}
				}
			}
		} );
}
