#include "Fpch.h"
#include "ISystem.h"

IMPLEMENT_TYPE(systems, ISystem)
IMPLEMENT_TYPE(systems, IECSSystem)

void systems::Archetype::MoveObjectTo( forge::ObjectID objectId, Archetype* destination )
{
	*std::find( m_sparseSet.begin(), m_sparseSet.end(), m_dataSize - 1u ) = m_sparseSet[ objectId.m_id ];

	for( auto& data : m_data )
	{
		auto& destinationPackage = destination->GetData( *data.first );
		data.second->MoveTo( m_sparseSet[ objectId.m_id ], destinationPackage );
	}

	--m_dataSize;
	m_sparseSet[ objectId.m_id ] = -1;
}

void systems::Archetype::MoveObjectTo( forge::ObjectID objectId, std::vector< std::unique_ptr< forge::IDataPackage > >& destination )
{
	destination.reserve( m_data.size() );
	*std::find( m_sparseSet.begin(), m_sparseSet.end(), m_dataSize - 1u ) = m_sparseSet[ objectId.m_id ];

	for( auto& data : m_data )
	{
		auto newData = data.second->CreateNewInstance();
		data.second->MoveTo( m_sparseSet[ objectId.m_id ], *newData );

		destination.emplace_back( std::move( newData ) );
	}

	--m_dataSize;
	m_sparseSet[ objectId.m_id ] = -1;
}

void systems::Archetype::MoveObjectFrom( forge::ObjectID objectId, std::vector< Archetype* > donorArchetypes )
{
	FORGE_ASSERT( m_sparseSet[ objectId.m_id ] == -1 );

	for( auto donor : donorArchetypes )
	{
		donor->MoveObjectTo( objectId, this );
	}

	m_sparseSet[ objectId.m_id ] = m_dataSize++;
}

void systems::ISystem::Initialize( forge::EngineInstance& engineInstance )
{
	m_engineInstance = &engineInstance;
	OnInitialize();
}

void systems::ISystem::Deinitialize()
{
	OnDeinitialize();
}
