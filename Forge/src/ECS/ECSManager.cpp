#include "Fpch.h"
#include "ECSManager.h"

ecs::Archetype& ecs::ECSManager::UpdateEntityArchetype( EntityID entityID, const ArchetypeID& newID )
{
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
		targetArchetype->AddEntityCopy( entityID, *currentArchetype );
		currentArchetype->RemoveEntity( entityID );
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
