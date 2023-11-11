#include "Fpch.h"
#include "TransformSystem.h"
#include "../ECS/Query.h"

RTTI_IMPLEMENT_TYPE( systems::TransformSystem );

void systems::TransformSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::FrameEnd, std::bind( &systems::TransformSystem::Update, this ) );
}

void systems::TransformSystem::Update()
{
	auto& ecsManager = GetEngineInstance().GetECSManager();
	ecs::CommandsQueue cmdsQueue( ecsManager );

	{
		ecs::Query previousFrameTransformToUpdateQuery( ecsManager );
		previousFrameTransformToUpdateQuery.AddFragmentRequirement< forge::PreviousFrameTransformFragment >(ecs::Query::RequirementType::Included);
		previousFrameTransformToUpdateQuery.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
			{
				cmdsQueue.RemoveFragment( archetype.GetArchetypeID(), forge::PreviousFrameTransformFragment::GetTypeStatic() );
			} );
	}

	{
		ecs::Query previousFrameScaleToUpdateQuery( ecsManager );
		previousFrameScaleToUpdateQuery.AddFragmentRequirement< forge::PreviousFrameScaleFragment >( ecs::Query::RequirementType::Included );
		previousFrameScaleToUpdateQuery.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
			{
				cmdsQueue.RemoveFragment( archetype.GetArchetypeID(), forge::PreviousFrameScaleFragment::GetTypeStatic() );
			} );
	}

	cmdsQueue.Execute();
}
