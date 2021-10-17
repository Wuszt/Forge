#include "Fpch.h"
#include "EntitiesManager.h"

#ifdef FORGE_IMGUI_ENABLED
#include "IMGUISystem.h"
#include "../../External/imgui/imgui.h"
#endif
void forge::EntitiesManager::Initialize()
{
#ifdef FORGE_IMGUI_ENABLED
	m_debugOverlayToken = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetSystemsManager().RegisterToOnBootCallback( [ this ]()
	{	
		if( auto* imguiSystem = GetEngineInstance().GetSystemsManager().GetSystemPtr< systems::IMGUISystem >() )
		{
			m_debugOverlayToken = std::make_unique< forge::CallbackToken >( imguiSystem->AddOverlayListener( [ & ]()
			{
				ImGui::Text( "Entities amount: %u", GetEntitiesAmount() );
			} ) );
		}
	} ) );
#endif

	 m_tickToken = std::make_unique< CallbackToken >( m_engineInstance.GetUpdateManager().RegisterUpdateFunction( UpdateManager::BucketType::Present, std::bind( &forge::EntitiesManager::HandleRequests, this ) ) );
}

void forge::EntitiesManager::Deinitialize()
{
#ifdef FORGE_IMGUI_ENABLED
	m_debugOverlayToken = nullptr;
#endif

	m_tickToken = nullptr;
}

void forge::EntitiesManager::RemoveEntity( const EntityID& id )
{
	m_entities[ id ]->OnDetach();
	m_entities.erase( id );
}

void forge::EntitiesManager::HandleRequests()
{
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
