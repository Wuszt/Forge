#include "Fpch.h"
#include "PhysicsComponent.h"
#include "PhysicsSystem.h"
#include "TransformComponent.h"
#include "../Physics/PhysicsShape.h"
#include "PhysicsUserData.h"

RTTI_IMPLEMENT_TYPE( forge::PhysicsComponent );

RTTI_IMPLEMENT_TYPE( forge::PhysicsStaticFragment );
RTTI_IMPLEMENT_TYPE( forge::PhysicsStaticComponent );

RTTI_IMPLEMENT_TYPE( forge::PhysicsDynamicFragment );
RTTI_IMPLEMENT_TYPE( forge::PhysicsDynamicComponent );

void forge::PhysicsComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData )
{
	auto& physicsSystem = engineInstance.GetSystemsManager().GetSystem < systems::PhysicsSystem >();
	const forge::ObjectID objectId = GetOwner().GetObjectID();
	const ecs::EntityID entityId = engineInstance.GetObjectsManager().GetOrCreateEntityId( objectId );

	physics::UserData userData{ entityId, objectId };
	GetActor().Initialize( physicsSystem.GetPhysicsProxy(), static_cast< Uint32 >( physics::PhysicsGroupFlags::Default ), GetOwner().GetComponent< forge::TransformComponent >()->GetWorldTransform(), userData.GetAsPointerSizeType() );

	physicsSystem.RegisterActor( GetActor() );
}

void forge::PhysicsComponent::OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	auto& physicsSystem = engineInstance.GetSystemsManager().GetSystem < systems::PhysicsSystem >();
	physicsSystem.UnregisterActor( GetActor() );
}

void forge::PhysicsComponent::AddShape( physics::PhysicsShape&& shape )
{
	GetActor().AddShape( std::move( shape ) );
}

void forge::PhysicsComponent::SetGroup( physics::PhysicsGroupFlags group )
{
	GetActor().SetGroup( static_cast< Uint32 >( group ) );
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
