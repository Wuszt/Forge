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

bool systems::PhysicsSystem::PerformRaycast( const Vector3& start, const Vector3& direction, Float length, physics::RaycastResult& outResult )
{
	return m_scene->PerformRaycast( start, direction, length, outResult );
}

void systems::PhysicsSystem::Update()
{
	{
		ecs::Query dynamicsQuery;
		dynamicsQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
		dynamicsQuery.AddFragmentRequirement< forge::PhysicsDynamicFragment >( ecs::Query::RequirementType::Included );
		dynamicsQuery.AddFragmentRequirement< forge::PreviousFrameScaleFragment >( ecs::Query::RequirementType::Included );

		ecs::Query staticsQuery;
		staticsQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
		staticsQuery.AddFragmentRequirement< forge::PhysicsStaticFragment >( ecs::Query::RequirementType::Included );
		staticsQuery.AddFragmentRequirement< forge::PreviousFrameScaleFragment >( ecs::Query::RequirementType::Included );

		auto updateScaleFunc = [ & ]< class T >( ecs::ArchetypeView archetype )
		{
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
			auto physicsFragments = archetype.GetFragments< T >();
			auto prevScaleFragments = archetype.GetFragments< forge::PreviousFrameScaleFragment >();

			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				physicsFragments[ i ].m_actor.ChangeScale( prevScaleFragments[ i ].m_previousScale, transformFragments[ i ].m_scale );
			}
		};

		dynamicsQuery.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::ArchetypeView archetype )
			{
				updateScaleFunc.operator() < forge::PhysicsDynamicFragment > ( archetype );
			} );

		staticsQuery.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::ArchetypeView archetype )
			{
				updateScaleFunc.operator() < forge::PhysicsStaticFragment > ( archetype );
			} );
	}

	{
		ecs::Query dynamicsQuery;
		dynamicsQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
		dynamicsQuery.AddFragmentRequirement< forge::PhysicsDynamicFragment >( ecs::Query::RequirementType::Included );

		ecs::Query staticsQuery;
		staticsQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
		staticsQuery.AddFragmentRequirement< forge::PhysicsStaticFragment >( ecs::Query::RequirementType::Included );

		auto updateTransformFunc = [ & ]< class T >( ecs::ArchetypeView archetype )
		{
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
			auto physicsFragments = archetype.GetFragments< T >();

			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				physicsFragments[ i ].m_actor.SetTransform( transformFragments[ i ].m_transform );
			}
		};

		dynamicsQuery.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::ArchetypeView archetype )
			{
				updateTransformFunc.operator() < forge::PhysicsDynamicFragment > ( archetype );
			} );

		staticsQuery.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::ArchetypeView archetype )
			{
				updateTransformFunc.operator() < forge::PhysicsStaticFragment > ( archetype );
			} );

		m_scene->Simulate( GetEngineInstance().GetSystemsManager().GetSystem< systems::TimeSystem >().GetDeltaTime() );

		std::vector< std::pair< ecs::EntityID, Transform > > m_updatedEntities;
		dynamicsQuery.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::ArchetypeView archetype )
			{
				auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
		auto physicsFragments = archetype.GetFragments< forge::PhysicsDynamicFragment >();

		for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
		{
			m_updatedEntities.emplace_back( archetype.GetEntityIDWithIndex( i ), transformFragments[ i ].m_transform );
			transformFragments[ i ].m_transform = physicsFragments[ i ].m_actor.GetTransform();
		}
			} );

		for ( auto& updatedEntity : m_updatedEntities )
		{
			if ( GetEngineInstance().GetECSManager().GetFragment< forge::PreviousFrameTransformFragment >( updatedEntity.first ) == nullptr )
			{
				GetEngineInstance().GetECSManager().AddFragmentToEntity< forge::PreviousFrameTransformFragment >( updatedEntity.first );
				GetEngineInstance().GetECSManager().GetFragment< forge::PreviousFrameTransformFragment >( updatedEntity.first )->m_previousTransform = updatedEntity.second;
			}
		}
	}
}

physics::UserData physics::UserData::GetFromRaycastResult( const RaycastResult& raycastResult )
{
	physics::UserData userData;
	static_assert( sizeof( userData ) == sizeof( raycastResult.m_userData ) );
	std::memcpy( &userData, &raycastResult.m_userData, sizeof( raycastResult.m_userData ) );
	return userData;
}
