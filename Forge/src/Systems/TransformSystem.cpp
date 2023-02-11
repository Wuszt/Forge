#include "Fpch.h"
#include "TransformSystem.h"
#include "../ECS/Query.h"

IMPLEMENT_TYPE( systems::TransformSystem );

void systems::TransformSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::FrameEnd, std::bind( &systems::TransformSystem::Update, this ) );
}

void systems::TransformSystem::Update()
{
	ecs::Query query;
	query.AddTagRequirement< forge::TransformModifiedThisFrame >( ecs::Query::RequirementType::Included );
	query.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );

	std::vector< ecs::EntityID > entities;

	query.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::Archetype& archetype )
		{
			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				entities.emplace_back( archetype.GetEntityIDWithIndex( i ) );
			}
		} );

	for ( const ecs::EntityID& entity : entities )
	{
		GetEngineInstance().GetECSManager().RemoveTagFromEntity< forge::TransformModifiedThisFrame >( entity );
	}
}
