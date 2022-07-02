#include "Fpch.h"
#include "ISystem.h"

#ifdef FORGE_DEBUGGING
#include "IMGUISystem.h"
#endif

IMPLEMENT_TYPE(systems, ISystem)
IMPLEMENT_TYPE(systems, IECSSystem)

void systems::Archetype::MoveEntityTo( forge::EntityID entityId, Archetype* destination )
{
	*std::find( m_sparseSet.begin(), m_sparseSet.end(), m_dataSize - 1u ) = m_sparseSet[ entityId.m_id ];

	for( auto& data : m_data )
	{
		auto& destinationPackage = destination->GetData( *data.first );
		data.second->MoveTo( m_sparseSet[ entityId.m_id ], destinationPackage );
	}

	--m_dataSize;
	m_sparseSet[ entityId.m_id ] = -1;
}

void systems::Archetype::MoveEntityTo( forge::EntityID entityId, std::vector< std::unique_ptr< forge::IDataPackage > >& destination )
{
	destination.reserve( m_data.size() );
	*std::find( m_sparseSet.begin(), m_sparseSet.end(), m_dataSize - 1u ) = m_sparseSet[ entityId.m_id ];

	for( auto& data : m_data )
	{
		auto newData = data.second->CreateNewInstance();
		data.second->MoveTo( m_sparseSet[ entityId.m_id ], *newData );

		destination.emplace_back( std::move( newData ) );
	}

	--m_dataSize;
	m_sparseSet[ entityId.m_id ] = -1;
}

void systems::Archetype::MoveEntityFrom( forge::EntityID entityId, std::vector< Archetype* > donorArchetypes )
{
	FORGE_ASSERT( m_sparseSet[ entityId.m_id ] == -1 );

	for( auto donor : donorArchetypes )
	{
		donor->MoveEntityTo( entityId, this );
	}

	m_sparseSet[ entityId.m_id ] = m_dataSize++;
}

#ifdef FORGE_DEBUGGING
void systems::ISystem::SetDebugAvailability( Bool available )
{
	if( available )
	{
		m_topBarHandle = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().GetTopBar().AddButton( { "Debug", "Systems", GetType().GetName( false ) }, true );
		m_onClickedTopBarItemToken = m_topBarHandle->GetCallback().AddListener( [ this ]()
		{
			if( m_topBarHandle->IsSelected() )
			{
				m_onRenderDebugToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, [ this ]()
				{
					OnRenderDebug();
				} );
			}
			else
			{
				m_onRenderDebugToken.Unregister();
			}
		} );
	}
	else
	{
		m_topBarHandle = nullptr;
		m_onRenderDebugToken.Unregister();
		m_onClickedTopBarItemToken.Unregister();
	}
}
#endif

void systems::ISystem::Initialize( forge::EngineInstance& engineInstance )
{
	m_engineInstance = &engineInstance;
	OnInitialize();
}

void systems::ISystem::Deinitialize()
{
	OnDeinitialize();
}
