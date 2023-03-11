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

void ecs::ECSManager::AddFragmentsAndTagsToEntity( EntityID entityID, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags )
{
	PC_SCOPE_FUNC();

	TagsFlags tagsFlags;
	for ( const auto* tagType : tags )
	{
		tagsFlags.Set( *tagType, true );
	}

	FragmentsFlags fragmentsFlags;
	for ( const auto* fragmentType : fragments )
	{
		fragmentsFlags.Set( *fragmentType, true );
	}

	AddFragmentsAndTagsToEntity( entityID, fragmentsFlags, tagsFlags );
}

void ecs::ECSManager::AddFragmentsAndTagsToEntity( EntityID entityID, FragmentsFlags fragments, TagsFlags tags )
{
	Archetype* currentArchetype = m_entityToArchetype[ entityID ];

	if ( ( fragments.Any() && tags.Any() )
		|| ( currentArchetype && currentArchetype->GetArchetypeID().ContainsAllTagsAndFragments( tags, fragments ) ) )
	{
		return;
	}

	ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();
	id.m_tagsFlags = id.m_tagsFlags | tags;
	id.m_fragmentsFlags = id.m_fragmentsFlags | fragments;

	MoveEntityToNewArchetype( entityID, id );
}

void ecs::ECSManager::MoveEntityToNewArchetype( EntityID entityID, const ArchetypeID& newID )
{
	PC_SCOPE_FUNC();

	Archetype* currentArchetype = m_entityToArchetype[ entityID ];
	Uint32 archetypeIndex = 0u;
	Archetype* targetArchetype = nullptr;
	if ( TryToFindArchetypeIndex( newID, archetypeIndex ) )
	{
		targetArchetype = m_archetypes[ archetypeIndex ].get();
	}
	else
	{
		std::vector< const ecs::Fragment::Type* > fragments;
		if ( currentArchetype )
		{
			newID.m_fragmentsFlags.VisitSetTypes( [ & ]( const ecs::Fragment::Type& type )
				{
					if ( !currentArchetype->ContainsFragment( type ) )
					{
						fragments.emplace_back( &type );
					}
				} );
		}

		targetArchetype = m_archetypes.emplace_back( std::make_unique< Archetype >( m_nextEntityID, newID ) ).get();
	}

	if ( currentArchetype )
	{
		targetArchetype->StealEntityFrom( entityID, *currentArchetype );

		if ( currentArchetype->IsEmpty() )
		{
			Uint32 currentArchetypeIndex = 0u;
			FORGE_ASSURE( TryToFindArchetypeIndex( currentArchetype->GetArchetypeID(), currentArchetypeIndex ) );
			forge::utils::RemoveReorder( m_archetypes, currentArchetypeIndex );
		}
	}
	else
	{
		targetArchetype->AddEntity( entityID );
	}

	m_entityToArchetype[ entityID ] = targetArchetype;
}

void ecs::ECSManager::RemoveFragmentFromEntity( EntityID entityID, const ecs::Fragment::Type& type )
{
	PC_SCOPE_FUNC();

	Archetype* currentArchetype = m_entityToArchetype[ entityID ];
	ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();

	if ( !id.ContainsFragment( type ) )
	{
		return;
	}

	id.RemoveFragment( type );

	MoveEntityToNewArchetype( entityID, id );
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

void ecs::ECSManager::RemoveTagFromEntity( EntityID entityID, const ecs::Tag::Type& type )
{
	PC_SCOPE_FUNC();

	Archetype* currentArchetype = m_entityToArchetype[ entityID ];
	ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();

	if ( !id.ContainsTag( type ) )
	{
		return;
	}

	id.RemoveTag( type );

	MoveEntityToNewArchetype( entityID, id );
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
