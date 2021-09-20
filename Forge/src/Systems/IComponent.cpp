#include "Fpch.h"
#include "IComponent.h"

forge::IComponent::IComponent( Entity& owner )
	: m_owner( owner )
{}
