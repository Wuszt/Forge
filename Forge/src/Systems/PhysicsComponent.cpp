#include "Fpch.h"
#include "PhysicsComponent.h"
#include "PhysicsSystem.h"
#include "TransformComponent.h"
#include "../Physics/PhysicsShape.h"

RTTI_IMPLEMENT_TYPE( forge::IPhysicsComponent );

RTTI_IMPLEMENT_TYPE( forge::PhysicsStaticFragment );
RTTI_IMPLEMENT_TYPE( forge::PhysicsStaticComponent );

RTTI_IMPLEMENT_TYPE( forge::PhysicsDynamicFragment );
RTTI_IMPLEMENT_TYPE( forge::PhysicsDynamicComponent );

void forge::IPhysicsComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	auto& physicsSystem = engineInstance.GetSystemsManager().GetSystem < systems::PhysicsSystem >();
	const forge::ObjectID objectId = GetOwner().GetObjectID();
	const ecs::EntityID entityId = engineInstance.GetObjectsManager().GetOrCreateEntityId( objectId );

	void* rawUserData = nullptr;
	physics::UserData userData{ entityId, objectId };
	static_assert( sizeof( rawUserData) == sizeof( userData ) );

	std::memcpy( &rawUserData, &userData, sizeof( userData ) );
	GetActor().Initialize( physicsSystem.GetPhysicsProxy(), GetOwner().GetComponent< forge::TransformComponent >()->GetWorldTransform(), rawUserData );

	physicsSystem.RegisterActor( GetActor() );
}

void forge::IPhysicsComponent::OnDetached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	auto& physicsSystem = engineInstance.GetSystemsManager().GetSystem < systems::PhysicsSystem >();
	physicsSystem.UnregisterActor( GetActor() );
}

void forge::IPhysicsComponent::AddShape( physics::PhysicsShape&& shape )
{
	GetActor().AddShape( std::move( shape ) );
}

physics::PhysicsStaticActor& forge::PhysicsStaticComponent::GetActor()
{
	return GetMutableData()->m_actor;
}

physics::PhysicsDynamicActor& forge::PhysicsDynamicComponent::GetActor()
{
	return GetMutableData()->m_actor;
}

void forge::PhysicsDynamicComponent::UpdateDensity( Float density )
{
	m_density = density;
	GetActor().SetDensity( m_density );
}

void forge::PhysicsDynamicComponent::AddShape( physics::PhysicsShape&& shape )
{
	Super::AddShape( std::move( shape ) );
	GetActor().SetDensity( m_density );
}
