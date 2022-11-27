#include "Fpch.h"
#include "IComponent.h"

IMPLEMENT_TYPE( forge, IComponent );

forge::IComponent::IComponent() = default;
forge::IComponent::~IComponent() = default;

void forge::IComponent::Attach( EngineInstance& engineInstance, Entity& owner )
{
	m_owner = &owner;
	OnAttach( engineInstance );
}

void forge::IComponent::Detach( EngineInstance& engineInstance )
{
	OnDetach( engineInstance );
	m_owner = nullptr;
}