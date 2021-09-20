#include "Fpch.h"
#include "EntitiesManager.h"

void forge::EntitiesManager::RemoveEntity( const EntityID& id )
{
	m_entities[ id ]->OnDetach();
	m_entities.erase( id );
}
