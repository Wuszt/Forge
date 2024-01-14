#include "Fpch.h"
#include "ObjectsManager.h"
void forge::ObjectsManager::RemoveObject( const ObjectID& id )
{
	m_objects[ id ]->OnDetach();
	m_objects.erase( id );

	auto foundEntityId = m_objectsToEntities.find( id );
	if ( foundEntityId != m_objectsToEntities.end() )
	{
		if ( m_ecsManager.DoesEntityExist( foundEntityId->second ) )
		{
			m_ecsManager.RemoveEntity( foundEntityId->second );
		}
		
		m_objectsToEntities.erase( foundEntityId );
	}

	m_onObjectDestructed.Invoke( id );
}

forge::ObjectsManager::ObjectsManager( EngineInstance& engineInstance, UpdateManager& updateManager, ecs::ECSManager& ecsManager )
	: m_engineInstance( engineInstance )
	, m_ecsManager( ecsManager )
{
	m_tickToken = updateManager.RegisterUpdateFunction( UpdateManager::BucketType::PreUpdate, std::bind( &forge::ObjectsManager::HandleRequests, this ) );
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

	{
		auto requests = std::move( m_objectDestructionRequests );
		m_objectDestructionRequests.clear();
		for ( const auto& request : requests )
		{
			RemoveObject( request.m_id );
		}
	}

	{
		auto requests = std::move( m_objectCreationRequests );
		m_objectCreationRequests.clear();
		for ( const auto& request : requests )
		{
			request.m_creationFunc();
		}
	}
}