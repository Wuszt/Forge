#include "Fpch.h"
#include "ObjectsManager.h"
void forge::ObjectsManager::RemoveObject( const ObjectID& id )
{
	m_objects[ id ]->OnDetach();
	m_objects.erase( id );
	m_onObjectDestructed.Invoke( id );
}

forge::ObjectsManager::ObjectsManager( EngineInstance& engineInstance, UpdateManager& updateManager, ecs::ECSManager& ecsManager )
	: m_engineInstance( engineInstance )
	, m_ecsManager( ecsManager )
{
	m_tickToken = updateManager.RegisterUpdateFunction( UpdateManager::BucketType::Present, std::bind( &forge::ObjectsManager::HandleRequests, this ) );
}

ecs::EntityID forge::ObjectsManager::GetOrCreateEntityId( ObjectID id )
{
	ecs::EntityID& entity = m_objectsToEntities[ id ];
	if( !entity.IsValid() )
	{
		entity = m_ecsManager.CreateEntity();
	}

	return entity;
}

void forge::ObjectsManager::HandleRequests()
{
	PC_SCOPE_FUNC();

	for( const auto& request : m_objectDestructionRequests )
	{
		RemoveObject( request.m_id );
	}

	m_objectDestructionRequests.clear();

	for( const auto& request : m_objectCreationRequests )
	{
		request.m_creationFunc();
	}

	m_objectCreationRequests.clear();

	for( const auto& request : m_componentCreationRequests )
	{
		request.m_creationFunc();
	}

	m_componentCreationRequests.clear();
}