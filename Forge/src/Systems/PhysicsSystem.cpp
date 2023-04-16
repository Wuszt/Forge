#include "Fpch.h"
#include "PhysicsSystem.h"
#include "../Physics/PhysxProxy.h"
#include "../ECS/Query.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "TimeSystem.h"
#include "../Physics/PhysicsScene.h"

IMPLEMENT_TYPE( systems::PhysicsSystem );

systems::PhysicsSystem::PhysicsSystem() = default;
systems::PhysicsSystem::PhysicsSystem( PhysicsSystem&& ) = default;
systems::PhysicsSystem::~PhysicsSystem() = default;

void systems::PhysicsSystem::OnInitialize()
{
	m_physicsProxy = std::make_unique< physics::PhysxProxy >();
	m_scene = std::make_unique<physics::PhysicsScene>( *m_physicsProxy );
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostUpdate, std::bind( &systems::PhysicsSystem::Update, this ) );
}

void systems::PhysicsSystem::OnDeinitialize()
{
	m_scene.reset();
	m_physicsProxy.reset();
}

void systems::PhysicsSystem::RegisterActor( physics::PhysicsActor& actor )
{
	m_scene->AddActor( actor );
}

void systems::PhysicsSystem::UnregisterActor( physics::PhysicsActor& actor )
{
	m_scene->RemoveActor( actor );
}

void systems::PhysicsSystem::Update()
{
	ecs::Query dynamicsQuery;
	dynamicsQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
	dynamicsQuery.AddFragmentRequirement< forge::PhysicsDynamicFragment >( ecs::Query::RequirementType::Included );

	ecs::Query staticsQuery;
	staticsQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
	staticsQuery.AddFragmentRequirement< forge::PhysicsStaticFragment >( ecs::Query::RequirementType::Included );

	auto updateFunc = [ & ]< class T >( ecs::Archetype& archetype )
	{
		auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
		auto physicsFragments = archetype.GetFragments< T >();

		for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
		{
			physicsFragments[ i ].m_actor.SetTransform( transformFragments[ i ].m_transform );
		}
	};

	dynamicsQuery.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::Archetype& archetype )
		{
			updateFunc.operator()< forge::PhysicsDynamicFragment >( archetype );
		} );

	staticsQuery.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::Archetype& archetype )
		{
			updateFunc.operator() < forge::PhysicsStaticFragment > ( archetype );
		} );

	m_scene->Simulate( GetEngineInstance().GetSystemsManager().GetSystem< systems::TimeSystem >().GetDeltaTime() );

	std::vector< ecs::EntityID > m_updatedEntities;
	dynamicsQuery.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::Archetype& archetype )
		{
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
			auto physicsFragments = archetype.GetFragments< forge::PhysicsDynamicFragment >();

			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				transformFragments[ i ].m_transform = physicsFragments[ i ].m_actor.GetTransform();
				m_updatedEntities.emplace_back( archetype.GetEntityIDWithIndex( i ) );
			}
		} );

	for ( const ecs::EntityID entityID : m_updatedEntities )
	{
		GetEngineInstance().GetECSManager().AddTagToEntity< forge::TransformModifiedThisFrame >( entityID );
	}
}
