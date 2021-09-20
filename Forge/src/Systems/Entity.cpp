#include "Fpch.h"
#include "Entity.h"

forge::Entity::Entity( EntitiesManager& entitiesManager, EntityID id )
	: m_entitiesManager( entitiesManager )
	, m_id( id )
{}

void forge::Entity::OnDetach()
{
	for( auto& comp : m_components )
	{
		comp.second->OnDetach();
	}
}
