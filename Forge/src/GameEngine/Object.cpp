#include "Fpch.h"
#include "Object.h"

forge::Object::Object( EngineInstance& engineInstance, ObjectID id )
	: m_engineInstance( engineInstance )
	, m_id( id )
{}

forge::Object::~Object() = default;

void forge::Object::OnDetach()
{
	for( auto& comp : m_components )
	{
		comp.second->Detach( m_engineInstance );
	}
}

void forge::Object::RequestAddingComponentsInternal( const std::function< void() >& creationFunc )
{
	GetEngineInstance().GetObjectsManager().RequestAddingComponentsToObject( creationFunc );
}
