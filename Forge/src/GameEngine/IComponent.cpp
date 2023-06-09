#include "Fpch.h"
#include "IComponent.h"

RTTI_IMPLEMENT_TYPE( forge::IComponent );

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