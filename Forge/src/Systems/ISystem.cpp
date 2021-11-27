#include "Fpch.h"
#include "ISystem.h"

void systems::Archetype::RemoveEntity( forge::EntityID id )
{
	*std::find( m_sparseSet.begin(), m_sparseSet.end(), m_dataSize - 1u ) = m_sparseSet[ id.m_id ];

	for( auto& data : m_data )
	{
		data.second->RemoveDataReorder( m_sparseSet[ id.m_id ] );
	}

	--m_dataSize;
}

void systems::Archetype::AddEntity( forge::EntityID id )
{
	FORGE_ASSERT( m_sparseSet[ id.m_id ] == -1 );

	for( auto& data : m_data )
	{
		data.second->AddEmptyData();
	}

	m_sparseSet[ id.m_id ] = m_dataSize++;
}

void systems::Archetype::MoveEntityTo( forge::EntityID entityId, Archetype* destination )
{
	*std::find( m_sparseSet.begin(), m_sparseSet.end(), m_dataSize - 1u ) = m_sparseSet[ entityId.m_id ];

	for( auto& data : m_data )
	{
		auto& destinationPackage = destination->GetData( data.first );
		data.second->MoveTo( m_sparseSet[ entityId.m_id ], destinationPackage );
	}

	--m_dataSize;
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
