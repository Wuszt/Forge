#include "Fpch.h"
#include "PhysicsComponent.h"
#include "PhysicsSystem.h"
#include "TransformComponent.h"

IMPLEMENT_TYPE( forge::IPhysicsComponent );

IMPLEMENT_TYPE( forge::PhysicsStaticFragment );
IMPLEMENT_TYPE( forge::PhysicsStaticComponent );

IMPLEMENT_TYPE( forge::PhysicsDynamicFragment );
IMPLEMENT_TYPE( forge::PhysicsDynamicComponent );

void forge::IPhysicsComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	auto& physicsSystem = engineInstance.GetSystemsManager().GetSystem < systems::PhysicsSystem >();
	GetActor().Initialize( physicsSystem.GetPhysicsProxy(), GetOwner().GetComponent< forge::TransformComponent >()->GetTransform() );

	physicsSystem.RegisterActor( GetActor() );
}

void forge::IPhysicsComponent::OnDetached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	auto& physicsSystem = engineInstance.GetSystemsManager().GetSystem < systems::PhysicsSystem >();
	physicsSystem.UnregisterActor( GetActor() );
}

void forge::IPhysicsComponent::AddShape( const physics::PhysicsShape& shape )
{
	GetActor().AddShape( shape );
}

physics::PhysicsStaticActor& forge::PhysicsStaticComponent::GetActor()
{
	return GetMutableData().m_actor;
}

physics::PhysicsDynamicActor& forge::PhysicsDynamicComponent::GetActor()
{
	return GetMutableData().m_actor;
}

void forge::PhysicsDynamicComponent::UpdateDensity( Float density )
{
	m_density = density;
	GetActor().UpdateDensity( m_density );
}

void forge::PhysicsDynamicComponent::AddShape( const physics::PhysicsShape& shape )
{
	Super::AddShape( shape );
	GetActor().UpdateDensity( m_density );
}
