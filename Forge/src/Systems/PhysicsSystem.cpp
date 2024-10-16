#include "Fpch.h"
#include "PhysicsSystem.h"
#include "../Physics/PhysxProxy.h"
#include "../ECS/Query.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "TimeSystem.h"
#include "../Physics/PhysicsScene.h"
#include "../Physics/RaycastResult.h"

RTTI_IMPLEMENT_TYPE( systems::PhysicsSystem );

systems::PhysicsSystem::PhysicsSystem() = default;
systems::PhysicsSystem::PhysicsSystem( PhysicsSystem&& ) = default;
systems::PhysicsSystem::~PhysicsSystem() = default;

void systems::PhysicsSystem::OnInitialize()
{
	m_physicsProxy = std::make_unique< physics::PhysxProxy >();
	m_scene = std::make_unique< physics::PhysicsScene >( *m_physicsProxy );
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostUpdate, std::bind( &systems::PhysicsSystem::UpdateSimulation, this ) );
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

bool systems::PhysicsSystem::PerformRaycast( const Vector3& start, const Vector3& direction, Float length, physics::RaycastResult& outResult, physics::PhysicsGroupFlags flags )
{
	UpdateScene();

	return m_scene->PerformRaycast( start, direction, length, static_cast< Uint32 >( flags ), outResult );
}

void systems::PhysicsSystem::UpdateSimulation()
{
	UpdateScene();

	m_scene->Simulate( GetEngineInstance().GetSystemsManager().GetSystem< systems::TimeSystem >().GetDeltaTime() );

	ecs::Query dynamicsQuery( GetEngineInstance().GetECSManager() );
	dynamicsQuery.AddMutableFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
	dynamicsQuery.AddFragmentRequirement< forge::PhysicsDynamicFragment >( ecs::Query::RequirementType::Included );

	std::vector< std::pair< ecs::EntityID, Transform > > m_updatedEntities;
	dynamicsQuery.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype )
		{
			auto transformFragments = archetype.GetMutableFragments< forge::TransformFragment >();
			auto physicsFragments = archetype.GetFragments< forge::PhysicsDynamicFragment >();

			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				m_updatedEntities.emplace_back( archetype.GetEntityIDWithIndex( i ), transformFragments[ i ].m_transform );
				transformFragments[ i ].m_transform = physicsFragments[ i ].m_actor.GetTransform();
			}
		} );
}

void systems::PhysicsSystem::UpdateScene()
{
	{
		ecs::Query dynamicsQuery( GetEngineInstance().GetECSManager() );
		dynamicsQuery.AddTagRequirement< forge::TransformWasModifiedThisFrame >( ecs::Query::RequirementType::Included );
		dynamicsQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
		dynamicsQuery.AddMutableFragmentRequirement< forge::PhysicsDynamicFragment >( ecs::Query::RequirementType::Included );

		ecs::Query staticsQuery( GetEngineInstance().GetECSManager() );
		staticsQuery.AddTagRequirement< forge::TransformWasModifiedThisFrame >( ecs::Query::RequirementType::Included );
		staticsQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
		staticsQuery.AddMutableFragmentRequirement< forge::PhysicsStaticFragment >( ecs::Query::RequirementType::Included );

		auto updateTransformFunc = [ & ]< class T >( ecs::MutableArchetypeView archetype )
		{
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
			auto physicsFragments = archetype.GetMutableFragments< T >();

			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				physicsFragments[ i ].m_actor.SetTransform( transformFragments[ i ].m_transform );
				physicsFragments[ i ].m_actor.ChangeScale( transformFragments[ i ].m_scale );
			}
		};

		dynamicsQuery.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype )
			{
				updateTransformFunc.operator() < forge::PhysicsDynamicFragment > ( archetype );
			} );

		staticsQuery.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype )
			{
				updateTransformFunc.operator() < forge::PhysicsStaticFragment > ( archetype );
			} );
	}
}