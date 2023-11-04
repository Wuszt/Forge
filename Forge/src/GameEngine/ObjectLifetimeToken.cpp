#include "Fpch.h"
#include "ObjectLifetimeToken.h"
#include "ObjectsManager.h"
#include "Object.h"

forge::ObjectLifetimeToken::ObjectLifetimeToken( ObjectID id, ObjectsManager& objectsManager )
	: m_objectId( id )
	, m_objectsManager( &objectsManager )
{}

forge::ObjectLifetimeToken::ObjectLifetimeToken( ObjectLifetimeToken&& other )
	: forge::ObjectLifetimeToken( other.m_objectId, *other.m_objectsManager )
{
	other.m_objectId = ObjectID();
}

forge::ObjectLifetimeToken::ObjectLifetimeToken( const Object& object )
	: forge::ObjectLifetimeToken( object.GetObjectID(), object.GetEngineInstance().GetObjectsManager() )
{}

forge::ObjectLifetimeToken::~ObjectLifetimeToken()
{
	if ( m_objectId.IsValid() )
	{
		m_objectsManager->RequestDestructingObject( m_objectId );
	}
}

forge::ObjectLifetimeToken& forge::ObjectLifetimeToken::operator=( ObjectLifetimeToken&& other )
{
	this->~ObjectLifetimeToken();
	m_objectId = other.m_objectId;
	m_objectsManager = other.m_objectsManager;
	other.m_objectId = ObjectID();
	return *this;
}