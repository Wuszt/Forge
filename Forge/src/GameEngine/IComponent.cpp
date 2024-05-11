#include "Fpch.h"
#include "IComponent.h"

RTTI_IMPLEMENT_TYPE( forge::IComponent );
RTTI_IMPLEMENT_TYPE( forge::IDataComponent );

forge::IComponent::IComponent() = default;
forge::IComponent::~IComponent() = default;

void forge::IComponent::Attach( EngineInstance& engineInstance, Object& owner, ecs::CommandsQueue& commandsQueue )
{
	m_owner = &owner;
	OnAttaching( engineInstance, commandsQueue );
}

void forge::IComponent::Detach( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue )
{
	OnDetaching( engineInstance, commandsQueue );
	m_owner = nullptr;
}

void forge::datacomponent::internal::OnAttaching( forge::Object& owner, const ecs::Fragment::Type& fragmentType, ecs::CommandsQueue& commandsQueue )
{
	auto& engineInstance = owner.GetEngineInstance();
	auto& ecsManager = engineInstance.GetECSManager();
	commandsQueue.AddFragment( GetObjectEntityID( owner ), fragmentType );
}

void forge::datacomponent::internal::OnDetaching( forge::Object& owner,const ecs::Fragment::Type& fragmentType, ecs::CommandsQueue& commandsQueue )
{
	auto& engineInstance = owner.GetEngineInstance();
	auto& ecsManager = engineInstance.GetECSManager();
	commandsQueue.RemoveFragment( GetObjectEntityID( owner ), fragmentType );
}

ecs::EntityID forge::datacomponent::internal::GetObjectEntityID( forge::Object& owner )
{
	auto& engineInstance = owner.GetEngineInstance();
	auto& objectsManager = engineInstance.GetObjectsManager();
	return objectsManager.GetOrCreateEntityId( owner.GetObjectID() );
}
