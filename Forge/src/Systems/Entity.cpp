#include "Fpch.h"
#include "Entity.h"

forge::Entity::Entity( EngineInstance& engineInstance, EntityID id )
	: m_engineInstance( engineInstance )
	, m_id( id )
{}

forge::Entity::~Entity() = default;

void forge::Entity::OnDetach()
{
	for( auto& comp : m_components )
	{
		comp.second->OnDetach( m_engineInstance );
	}
}
