#include "Fpch.h"
#include "TransformSystem.h"
#include "../ECS/Query.h"

RTTI_IMPLEMENT_TYPE( systems::TransformSystem );

void systems::TransformSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::FrameEnd, [ this ]() { systems::TransformSystem::Update(); } );

	m_modifyingArchetypeToken = GetEngineInstance().GetECSManager().RegisterModifyingArchetypeObserver( forge::TransformFragment::GetTypeStatic(), [ this ]( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue )
		{
			OnBeforeModifyingTransforms( archetypeView, commandsQueue );
		},
		ecs::FragmentsFlags{ &forge::TransformFragment::GetTypeStatic() },
		ecs::FragmentsFlags{ &forge::PreviousFrameTransformFragment::GetTypeStatic() } );

	m_modifyingEntityToken = GetEngineInstance().GetECSManager().RegisterModifyingFragmentObserver( forge::TransformFragment::GetTypeStatic(), [ this ]( ecs::EntityID id )
		{
			OnBeforeModifyingTransform( id );
		} );
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

	cmdsQueue.Execute();
}

void systems::TransformSystem::OnBeforeModifyingTransforms( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue )
{
	if ( !archetypeView.GetArchetypeID().ContainsFragment< forge::PreviousFrameTransformFragment >() )
	{
		commandsQueue.AddFragment( archetypeView.GetArchetypeID(), forge::PreviousFrameTransformFragment::GetTypeStatic() );
	}

	m_postCommandQueueExecutionToken = commandsQueue.AddPostExecutionCallback( [ archetypeView ]()
		{
			auto previousTransforms = archetypeView.GetMutableFragments< forge::PreviousFrameTransformFragment >();
			auto currentTransforms = archetypeView.GetFragments< forge::TransformFragment >();

			for ( Uint32 i = 0u; i < archetypeView.GetEntitiesAmount(); ++i )
			{
				previousTransforms[ i ].m_transform = currentTransforms[ i ].m_transform;
				previousTransforms[ i ].m_scale = currentTransforms[ i ].m_scale;
			}
		} );
}

void systems::TransformSystem::OnBeforeModifyingTransform( ecs::EntityID id )
{
	ecs::ECSManager& ecsManager = GetEngineInstance().GetECSManager();
	const auto& transformFragment = *ecsManager.GetFragment< forge::TransformFragment >( id );

	if ( ecsManager.GetFragment< forge::PreviousFrameTransformFragment >( id ) == nullptr )
	{
		ecsManager.AddFragmentDataToEntity( id, forge::PreviousFrameTransformFragment( transformFragment.m_transform, transformFragment.m_scale ) );
	}
}
