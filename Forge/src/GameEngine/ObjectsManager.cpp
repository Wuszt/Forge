#include "Fpch.h"
#include "ObjectsManager.h"

void forge::ObjectsManager::Initialize()
{
	 m_tickToken = m_engineInstance.GetUpdateManager().RegisterUpdateFunction( UpdateManager::BucketType::Present, std::bind( &forge::ObjectsManager::HandleRequests, this ) );
}

void forge::ObjectsManager::Deinitialize()
{
#ifdef FORGE_IMGUI_ENABLED
	m_debugOverlayToken.Unregister();
#endif

	m_tickToken.Unregister();
}

void forge::ObjectsManager::RemoveObject( const ObjectID& id )
{
	m_objects[ id ]->OnDetach();
	m_objects.erase( id );
	m_onObjectDestructed.Invoke( id );
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
