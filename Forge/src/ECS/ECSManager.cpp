#include "Fpch.h"
#include "ECSManager.h"

ecs::EntityID ecs::ECSManager::CreateEntity()
{
	PC_SCOPE_FUNC();

	for( auto& archetype : m_archetypes )
	{
		archetype->OnEntityCreated();
	}

	return ecs::EntityID( m_nextEntityID++ );}

void ecs::ECSManager::RemoveEntity( EntityID id )
{
	PC_SCOPE_FUNC();

	auto it = m_entityToArchetype.find( id );
	if( it != m_entityToArchetype.end() )
	{
		it->second->RemoveEntity( id );
		if( it->second->IsEmpty() )
		{
			Uint32 currentArchetypeIndex = 0u;
			FORGE_ASSURE( TryToFindArchetypeIndex( it->second->GetArchetypeID(), currentArchetypeIndex ) );
			forge::utils::RemoveReorder( m_archetypes, currentArchetypeIndex );
		}

		m_entityToArchetype.erase( it );
	}
}

ecs::Archetype& ecs::ECSManager::UpdateEntityArchetype( EntityID entityID, const ArchetypeID& newID )
{
	PC_SCOPE_FUNC();

	Archetype* currentArchetype = m_entityToArchetype[ entityID ];
	Archetype* targetArchetype = nullptr;
	Uint32 archetypeIndex = 0u;
	if( TryToFindArchetypeIndex( newID, archetypeIndex ) )
	{
		targetArchetype = m_archetypes[ archetypeIndex ].get();
	}
	else
	{
		targetArchetype = currentArchetype ? m_archetypes.emplace_back( std::make_unique< Archetype >( currentArchetype->GetEmptyCopy() ) ).get() : m_archetypes.emplace_back( std::make_unique< Archetype >( m_nextEntityID ) ).get();
	}

	FORGE_ASSERT( currentArchetype != targetArchetype );

	if( currentArchetype )
	{
		targetArchetype->StealEntityFrom( entityID, *currentArchetype );
	}
	else
	{
		targetArchetype->AddEntity( entityID );
	}

	if( currentArchetype && currentArchetype->IsEmpty() )
	{
		Uint32 currentArchetypeIndex = 0u;
		FORGE_ASSURE( TryToFindArchetypeIndex( currentArchetype->GetArchetypeID(), currentArchetypeIndex ) );
		forge::utils::RemoveReorder( m_archetypes, currentArchetypeIndex );
	}

	m_entityToArchetype[ entityID ] = targetArchetype;

	return *targetArchetype;
}

ecs::Archetype* ecs::ECSManager::GetEntityArchetype( EntityID id )
{
	auto it = m_entityToArchetype.find( id );
	if( it != m_entityToArchetype.end() )
	{
		return it->second;
	}

	return nullptr;
}

Bool ecs::ECSManager::TryToFindArchetypeIndex( ArchetypeID Id, Uint32& outIndex ) const
{
	auto it = std::find_if( m_archetypes.begin(), m_archetypes.end(), [ &Id ]( const std::unique_ptr< Archetype >& archetype )
	{
		return archetype->GetArchetypeID() == Id;
	} );

	outIndex = static_cast< Uint32 >( it - m_archetypes.begin() );

	return it != m_archetypes.end();
}
