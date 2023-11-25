#include "Fpch.h"
#include "TransformSystem.h"
#include "../ECS/Query.h"

RTTI_IMPLEMENT_TYPE( systems::TransformSystem );

// TODO: SetParent kiedy parent jest juz dirty

template< class Callable >
static void CallWithCachedCommandsQueue( const Callable& func, ecs::ECSManager& ecsManager, ecs::CommandsQueue* &currentCommandsQueue)
{
	if ( currentCommandsQueue == nullptr )
	{
		ecs::CommandsQueue commandsQueue( ecsManager );
		currentCommandsQueue = &commandsQueue;

		func( *currentCommandsQueue );

		currentCommandsQueue = nullptr;
		commandsQueue.Execute();
	}
	else
	{
		func( *currentCommandsQueue );
	}
}

template< class Callable >
static void CallWithCachedCommandsQueue( const Callable& func, ecs::ECSManager& ecsManager, ecs::CommandsQueue*& currentCommandsQueue, ecs::CommandsQueue& commandsQueue )
{
	if ( currentCommandsQueue == nullptr )
	{
		currentCommandsQueue = &commandsQueue;

		func( *currentCommandsQueue );

		currentCommandsQueue = nullptr;
	}
	else
	{
		func( *currentCommandsQueue );
	}
}

void systems::TransformSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::FrameEnd, [ this ]() { systems::TransformSystem::Update(); } );

	m_modifyingArchetypeToken = GetEngineInstance().GetECSManager().RegisterModifyingArchetypeObserver< forge::TransformFragment >( [ this ]( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue )
		{
			OnModifyingTransformArchetype( archetypeView, commandsQueue );
		},
		ecs::FragmentsFlags{ &forge::TransformFragment::GetTypeStatic(), &forge::TransformParentFragment::GetTypeStatic() },
		{}
		);

	m_modifyingEntityToken = GetEngineInstance().GetECSManager().RegisterModifyingFragmentObserver< forge::TransformFragment >( [ this ]( ecs::MutableFragmentUntypedView&& fragment )
		{
			OnModifyingTransformFragment( std::move( fragment ) );
		} );

	m_modifyingChildArchetypeToken = GetEngineInstance().GetECSManager().RegisterModifyingArchetypeObserver< forge::TransformChildFragment >( [ this ]( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue )
		{
			OnModifyingChildTransformArchetype( archetypeView, commandsQueue );
		},
		ecs::FragmentsFlags{ &forge::TransformFragment::GetTypeStatic(), &forge::TransformParentFragment::GetTypeStatic() },
		{}
		);

	m_modifyingChildEntityToken = GetEngineInstance().GetECSManager().RegisterModifyingFragmentObserver< forge::TransformChildFragment >( [ this ]( ecs::MutableFragmentUntypedView&& fragment )
		{
			OnModifyingChildTransformFragment( std::move( fragment ) );
		} );

	m_readingArchetypeToken = GetEngineInstance().GetECSManager().RegisterReadArchetypeObserver< forge::TransformFragment >( [ this ]( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue )
		{
			OnReadingTransformArchetype( archetypeView, commandsQueue );
		},
		ecs::FragmentsFlags{ &forge::TransformChildFragment::GetTypeStatic(), &forge::TransformParentFragment::GetTypeStatic() },
		ecs::FragmentsFlags{ &forge::TransformFragment::GetTypeStatic() }
		);

	m_readingEntityToken = GetEngineInstance().GetECSManager().RegisterReadFragmentObserver< forge::TransformFragment > ( [ this ]( ecs::MutableFragmentUntypedView&& fragment )
		{
			OnReadingTransformFragment( std::move( fragment ) );
		} );

	m_readingChildArchetypeToken = GetEngineInstance().GetECSManager().RegisterReadArchetypeObserver< forge::TransformChildFragment >( [ this ]( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue )
		{
			OnReadingChildTransformArchetype( archetypeView, commandsQueue );
		},
		ecs::FragmentsFlags{ &forge::TransformChildFragment::GetTypeStatic(), &forge::TransformParentFragment::GetTypeStatic() },
		ecs::FragmentsFlags{ &forge::TransformFragment::GetTypeStatic() }
		);

	m_readingChildEntityToken = GetEngineInstance().GetECSManager().RegisterReadFragmentObserver< forge::TransformChildFragment >( [ this ]( ecs::MutableFragmentUntypedView&& fragment )
		{
			OnReadingChildTransformFragment( std::move( fragment ) );
		} );
}

void systems::TransformSystem::Update()
{
	auto& ecsManager = GetEngineInstance().GetECSManager();
	ecs::CommandsQueue cmdsQueue( ecsManager );

	{
		ecs::Query previousFrameTransformToUpdateQuery( ecsManager );
		previousFrameTransformToUpdateQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
		previousFrameTransformToUpdateQuery.AddTagRequirement< forge::ChildRequiringRecalculatingLocalTransform >( ecs::Query::RequirementType::Included );
		previousFrameTransformToUpdateQuery.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
			{
				cmdsQueue.RemoveTag( archetype.GetArchetypeID(), forge::TransformWasModifiedThisFrame::GetTypeStatic() );
			} );
	}

	cmdsQueue.Execute();
}

void systems::TransformSystem::RecalculateWorldTransform( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue )
{
	commandsQueue.RemoveTag( archetypeView.GetArchetypeID(), forge::ChildRequiringRecalculatingWorldTransform::GetTypeStatic() );
	auto childFragments = archetypeView.GetFragments< forge::TransformChildFragment >();
	auto transformFragments = archetypeView.GetMutableFragments< forge::TransformFragment >();

	for ( Uint32 i = 0u; i < archetypeView.GetEntitiesAmount(); ++i )
	{
		auto parentTransformFragment = archetypeView.GetFragmentView< forge::TransformFragment >( childFragments[ i ].m_parentId );
		if ( !parentTransformFragment )
		{
			parentTransformFragment = GetEngineInstance().GetECSManager().GetFragmentView< forge::TransformFragment >( childFragments[ i ].m_parentId );
		}

		transformFragments[ i ].m_transform = Transform( childFragments[ i ].ToMatrix() * parentTransformFragment->ToMatrix() );
		transformFragments[ i ].m_scale = parentTransformFragment->m_scale * childFragments[ i ].m_scale;
	}
}

void systems::TransformSystem::RecalculateLocalTransform( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue )
{
	commandsQueue.RemoveTag( archetypeView.GetArchetypeID(), forge::ChildRequiringRecalculatingLocalTransform::GetTypeStatic() );
	auto childFragments = archetypeView.GetMutableFragments< forge::TransformChildFragment >();
	auto transformFragments = archetypeView.GetFragments< forge::TransformFragment >();

	for ( Uint32 i = 0u; i < archetypeView.GetEntitiesAmount(); ++i )
	{
		auto parentTransformFragment = archetypeView.GetFragmentView< forge::TransformFragment >( childFragments[ i ].m_parentId );
		if ( !parentTransformFragment )
		{
			parentTransformFragment = GetEngineInstance().GetECSManager().GetFragmentView< forge::TransformFragment >( childFragments[ i ].m_parentId );
		}

		const Matrix localMatrix = transformFragments[ i ].ToMatrix() * parentTransformFragment->ToMatrix().OrthonormInverted();

		Vector3 localPos;
		Quaternion localRot;

		localMatrix.Decompose( childFragments[ i ].m_scale, localRot, localPos );
		childFragments[ i ].m_transform = Transform( localPos, localRot );
	}
}

void systems::TransformSystem::RecalculateLocalTransform( ecs::EntityID id, forge::TransformChildFragment& transformChildFragment, ecs::CommandsQueue& commandsQueue )
{
	ecs::ECSManager& ecsManager = GetEngineInstance().GetECSManager();
	commandsQueue.RemoveTag( id, forge::ChildRequiringRecalculatingLocalTransform::GetTypeStatic() );

	if ( transformChildFragment.m_parentId.IsValid() )
	{
		auto parentTransformFragment = GetEngineInstance().GetECSManager().GetFragmentView< forge::TransformFragment >( transformChildFragment.m_parentId );

		const Matrix localMatrix = ecsManager.GetFragmentView< forge::TransformFragment >( id )->ToMatrix() * parentTransformFragment->ToMatrix().OrthonormInverted();

		Vector3 localPos;
		Quaternion localRot;

		localMatrix.Decompose( transformChildFragment.m_scale, localRot, localPos );
		transformChildFragment.m_transform = Transform( localPos, localRot );
	}
}

void systems::TransformSystem::MakeDirty( const ecs::MutableArchetypeView& archetypeView, ecs::CommandsQueue& commandsQueue )
{
	commandsQueue.AddTag( archetypeView.GetArchetypeID(), forge::TransformWasModifiedThisFrame::GetTypeStatic() );

	if ( archetypeView.GetArchetypeID().ContainsFragment< forge::TransformParentFragment >() )
	{
		auto parentFragments = archetypeView.GetFragments< forge::TransformParentFragment >();
		for ( Uint32 i = 0u; i < archetypeView.GetEntitiesAmount(); ++i )
		{
			for ( ecs::EntityID childId : parentFragments[ i ].m_children )
			{
				if ( GetEngineInstance().GetECSManager().GetEntityArchetypeId( childId ).ContainsTag< forge::ChildRequiringRecalculatingLocalTransform >() )
				{
					GetEngineInstance().GetECSManager().GetFragmentView< forge::TransformChildFragment >( childId );
				}

				commandsQueue.AddTag( childId, forge::ChildRequiringRecalculatingWorldTransform::GetTypeStatic() );
				MakeDirty( childId, commandsQueue );
			}
		}
	}
}

void systems::TransformSystem::OnReadingTransformArchetype( ecs::MutableArchetypeView archetypeView, ecs::CommandsQueue& commandsQueue )
{
	if ( archetypeView.GetArchetypeID().ContainsTag< forge::ChildRequiringRecalculatingWorldTransform >() )
	{
		auto func = [ & ]( ecs::CommandsQueue& commandsQueue )
		{
			RecalculateWorldTransform( archetypeView, commandsQueue );
			archetypeView.Release();
		};

		CallWithCachedCommandsQueue( func, GetEngineInstance().GetECSManager(), m_currentCommandsQueue, commandsQueue );
	}
}

void systems::TransformSystem::OnReadingTransformFragment( ecs::MutableFragmentUntypedView fragment )
{
	if ( GetEngineInstance().GetECSManager().GetEntityArchetypeId( fragment.GetOwnerEntityId() ).ContainsTag< forge::ChildRequiringRecalculatingWorldTransform >() )
	{
		auto func = [ & ]( ecs::CommandsQueue& commandsQueue )
		{
			RecalculateWorldTransform( fragment.GetOwnerEntityId(), *fragment.GetMutableFragment< forge::TransformFragment >(), commandsQueue );
			fragment.Release();
		};

		CallWithCachedCommandsQueue( func, GetEngineInstance().GetECSManager(), m_currentCommandsQueue );
	}
}

void systems::TransformSystem::OnModifyingTransformArchetype( ecs::MutableArchetypeView archetypeView, ecs::CommandsQueue& commandsQueue )
{
	auto func = [ & ]( ecs::CommandsQueue& commandsQueue )
	{
		if ( archetypeView.GetArchetypeID().ContainsFragment< forge::TransformChildFragment >() )
		{
			commandsQueue.AddTag( archetypeView.GetArchetypeID(), forge::ChildRequiringRecalculatingLocalTransform::GetTypeStatic() );
		}

		MakeDirty( archetypeView, commandsQueue );
		archetypeView.Release();
	};

	CallWithCachedCommandsQueue( func, GetEngineInstance().GetECSManager(), m_currentCommandsQueue, commandsQueue );
}

void systems::TransformSystem::OnModifyingTransformFragment( ecs::MutableFragmentUntypedView fragment )
{
	auto func = [ & ]( ecs::CommandsQueue& commandsQueue )
	{
		if ( GetEngineInstance().GetECSManager().GetEntityArchetypeId( fragment.GetOwnerEntityId() ).ContainsFragment< forge::TransformChildFragment >() )
		{
			m_currentCommandsQueue->AddTag( fragment.GetOwnerEntityId(), forge::ChildRequiringRecalculatingLocalTransform::GetTypeStatic() );
		}

		MakeDirty( fragment.GetOwnerEntityId(), *m_currentCommandsQueue );
		fragment.Release();
	};

	CallWithCachedCommandsQueue( func, GetEngineInstance().GetECSManager(), m_currentCommandsQueue );
}

void systems::TransformSystem::OnReadingChildTransformArchetype( ecs::MutableArchetypeView archetypeView, ecs::CommandsQueue& commandsQueue )
{
	auto func = [ & ]( ecs::CommandsQueue& commandsQueue )
	{
		if ( archetypeView.GetArchetypeID().ContainsTag< forge::ChildRequiringRecalculatingLocalTransform >() )
		{
			RecalculateLocalTransform( archetypeView, commandsQueue );
		}
		archetypeView.Release();
	};

	CallWithCachedCommandsQueue( func, GetEngineInstance().GetECSManager(), m_currentCommandsQueue, commandsQueue );
}

void systems::TransformSystem::OnReadingChildTransformFragment( ecs::MutableFragmentUntypedView fragment )
{
	if ( GetEngineInstance().GetECSManager().GetEntityArchetypeId( fragment.GetOwnerEntityId() ).ContainsTag< forge::ChildRequiringRecalculatingLocalTransform >() )
	{
		auto func = [ & ]( ecs::CommandsQueue& commandsQueue )
		{
			RecalculateLocalTransform( fragment.GetOwnerEntityId(), *fragment.GetMutableFragment< forge::TransformChildFragment >(), commandsQueue );
			fragment.Release();
		};

		CallWithCachedCommandsQueue( func, GetEngineInstance().GetECSManager(), m_currentCommandsQueue );
	}
}

void systems::TransformSystem::OnModifyingChildTransformArchetype( ecs::MutableArchetypeView archetypeView, ecs::CommandsQueue& commandsQueue )
{
	auto func = [ & ]( ecs::CommandsQueue& commandsQueue )
	{
		commandsQueue.AddTag( archetypeView.GetArchetypeID(), forge::ChildRequiringRecalculatingWorldTransform::GetTypeStatic() );
		MakeDirty( archetypeView, commandsQueue );
		archetypeView.Release();
	};

	CallWithCachedCommandsQueue( func, GetEngineInstance().GetECSManager(), m_currentCommandsQueue, commandsQueue );
}

void systems::TransformSystem::OnModifyingChildTransformFragment( ecs::MutableFragmentUntypedView fragment )
{
	auto func = [ & ]( ecs::CommandsQueue& commandsQueue )
	{
		commandsQueue.AddTag( fragment.GetOwnerEntityId(), forge::ChildRequiringRecalculatingWorldTransform::GetTypeStatic() );
		MakeDirty( fragment.GetOwnerEntityId(), commandsQueue );
		fragment.Release();
	};

	CallWithCachedCommandsQueue( func, GetEngineInstance().GetECSManager(), m_currentCommandsQueue );
}

void systems::TransformSystem::MakeDirty( ecs::EntityID id, ecs::CommandsQueue& commandsQueue )
{
	commandsQueue.AddTag( id, forge::TransformWasModifiedThisFrame::GetTypeStatic() );

	auto& ecsManager = GetEngineInstance().GetECSManager();
	if ( auto parentFragment = ecsManager.GetFragmentView< forge::TransformParentFragment >( id ) )
	{
		for ( ecs::EntityID childId : parentFragment->m_children )
		{
			if ( GetEngineInstance().GetECSManager().GetEntityArchetypeId( childId ).ContainsTag< forge::ChildRequiringRecalculatingLocalTransform >() )
			{
				GetEngineInstance().GetECSManager().GetFragmentView< forge::TransformChildFragment >( childId );
			}

			commandsQueue.AddTag( childId, forge::ChildRequiringRecalculatingWorldTransform::GetTypeStatic() );
			MakeDirty( childId, commandsQueue );
		}
	}
}

void systems::TransformSystem::RecalculateWorldTransform( ecs::EntityID id, forge::TransformFragment& transformFragment, ecs::CommandsQueue& commandsQueue )
{
	ecs::ECSManager& ecsManager = GetEngineInstance().GetECSManager();
	commandsQueue.RemoveTag( id, forge::ChildRequiringRecalculatingWorldTransform::GetTypeStatic() );

	if( ecsManager.GetFragmentView< forge::TransformChildFragment >( id )->m_parentId.IsValid() )
	{
		const forge::TransformFragment& parentTransformFragment = *GetEngineInstance().GetECSManager().GetFragmentView< forge::TransformFragment >( ecsManager.GetFragmentView< forge::TransformChildFragment >( id )->m_parentId );

		auto childTransformFragment = ecsManager.GetFragmentView< forge::TransformChildFragment >( id );
		transformFragment.m_transform = Transform( childTransformFragment->ToMatrix() * parentTransformFragment.ToMatrix() );
		transformFragment.m_scale = parentTransformFragment.m_scale * ecsManager.GetFragmentView< forge::TransformChildFragment >( id )->m_scale;
	}
}
