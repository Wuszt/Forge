#include "Fpch.h"
#include "EntitiesManager.h"

void forge::EntitiesManager::Initialize()
{
	 m_tickToken = m_engineInstance.GetUpdateManager().RegisterUpdateFunction( UpdateManager::BucketType::Present, std::bind( &forge::EntitiesManager::HandleRequests, this ) );
}

void forge::EntitiesManager::Deinitialize()
{
#ifdef FORGE_IMGUI_ENABLED
	m_debugOverlayToken.Unregister();
#endif

	m_tickToken.Unregister();
}

void forge::EntitiesManager::RemoveEntity( const EntityID& id )
{
	m_entities[ id ]->OnDetach();
	m_entities.erase( id );
	m_onEntityDestructed.Invoke( id );
}

void forge::EntitiesManager::HandleRequests()
{
	PC_SCOPE_FUNC();

	for( const auto& request : m_entityDestructionRequests )
	{
		RemoveEntity( request.m_id );
	}

	m_entityDestructionRequests.clear();

	for( const auto& request : m_entityCreationRequests )
	{
		request.m_creationFunc();
	}

	m_entityCreationRequests.clear();

	for( const auto& request : m_componentCreationRequests )
	{
		request.m_creationFunc();
	}

	m_componentCreationRequests.clear();
}
