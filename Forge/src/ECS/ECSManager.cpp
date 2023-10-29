#include "Fpch.h"
#include "ECSManager.h"

ecs::EntityID ecs::ECSManager::CreateEntity()
{
	PC_SCOPE_FUNC();

	for ( auto& archetype : m_archetypes )
	{
		archetype->OnEntityCreated();
	}

	return ecs::EntityID( m_nextEntityID++ );
}

void ecs::ECSManager::RemoveEntity( EntityID id )
{
	PC_SCOPE_FUNC();

	auto it = m_entityToArchetype.find( id );
	if ( it != m_entityToArchetype.end() )
	{
		it->second->RemoveEntity( id );
		if ( it->second->IsEmpty() )
		{
			Uint32 currentArchetypeIndex = 0u;
			FORGE_ASSURE( TryToFindArchetypeIndex( it->second->GetArchetypeID(), currentArchetypeIndex ) );
			forge::utils::RemoveReorder( m_archetypes, currentArchetypeIndex );
		}

		m_entityToArchetype.erase( it );
	}
}

static ecs::TagsFlags ArraySpanToTags( forge::ArraySpan< const ecs::Tag::Type* > tags )
{
	ecs::TagsFlags tagsFlags;
	for ( const auto* tagType : tags )
	{
		tagsFlags.Set( *tagType, true );
	}

	return tagsFlags;
}

static ecs::FragmentsFlags ArraySpanToFragments( forge::ArraySpan< const ecs::Fragment::Type* > fragments )
{
	ecs::FragmentsFlags fragmentsFlags;
	for ( const auto* fragmentType : fragments )
	{
		fragmentsFlags.Set( *fragmentType, true );
	}

	return fragmentsFlags;
}

void ecs::ECSManager::AddFragmentsAndTagsToEntity( EntityID entityID, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags )
{
	PC_SCOPE_FUNC();

	AddFragmentsAndTagsToEntity( entityID, ArraySpanToFragments( fragments ), ArraySpanToTags( tags ) );
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
	id.AddTags( tags );
	id.AddFragments( fragments );

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
			newID.GetFragmentsFlags().VisitSetTypes( [ & ]( const ecs::Fragment::Type& type )
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

void ecs::ECSManager::SetArchetypeFragmentsAndTags( const ecs::ArchetypeID& archetypeId, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags )
{
	SetArchetypeFragmentsAndTags( archetypeId, ArraySpanToFragments( fragments ), ArraySpanToTags( tags ) );
}

void ecs::ECSManager::SetArchetypeFragmentsAndTags( const ecs::ArchetypeID& archetypeId, FragmentsFlags fragments, TagsFlags tags )
{
	Uint32 oldArchetypeIndex = 0u;
	FORGE_ASSURE( TryToFindArchetypeIndex( archetypeId, oldArchetypeIndex ) );

	ArchetypeID newId( fragments, tags );
	Uint32 newArchetypeIndex = 0u;
	if ( TryToFindArchetypeIndex( newId, newArchetypeIndex ) )
	{
		for ( Uint32 i = 0u; i < m_archetypes[ oldArchetypeIndex ]->GetEntitiesAmount(); ++i )
		{
			m_entityToArchetype.at( m_archetypes[ oldArchetypeIndex ]->GetEntityIDWithIndex( i ) ) = m_archetypes[ newArchetypeIndex ].get();
		}

		m_archetypes[ newArchetypeIndex ]->StealEntitiesFrom( *m_archetypes[ oldArchetypeIndex ] );
		forge::utils::RemoveReorder( m_archetypes, oldArchetypeIndex );
	}
	else
	{
		Archetype& oldArchetype = *m_archetypes[ oldArchetypeIndex ];
		oldArchetype.ClearTags();
		oldArchetype.AddTags( tags );

		FragmentsFlags fragmentsToAdd = archetypeId.GetFragmentsFlags().Flipped() & fragments;
		FragmentsFlags fragmentsToRemove = archetypeId.GetFragmentsFlags() & fragments.Flipped();

		fragmentsToAdd.VisitSetTypes( [ & ]( const ecs::Fragment::Type& type )
			{
				oldArchetype.AddFragmentType( type );
			} );

		fragmentsToRemove.VisitSetTypes( [ & ]( const ecs::Fragment::Type& type )
			{
				oldArchetype.RemoveFragmentType( type );
			} );
	}
}

void ecs::ECSManager::AddFragmentsAndTagsToArchetype( const ecs::ArchetypeID& archetypeId, FragmentsFlags newFragments, TagsFlags newTags )
{
	SetArchetypeFragmentsAndTags( archetypeId, archetypeId.GetFragmentsFlags() | newFragments, archetypeId.GetTagsFlags() | newTags );
}

void ecs::ECSManager::AddFragmentsAndTagsToArchetype( const ecs::ArchetypeID& archetypeId, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags )
{
	AddFragmentsAndTagsToArchetype( archetypeId, ArraySpanToFragments( fragments ), ArraySpanToTags( tags ) );
}

void ecs::ECSManager::RemoveFragmentsAndTagsFromArchetype( const ecs::ArchetypeID& archetypeId, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags )
{
	RemoveFragmentsAndTagsFromArchetype( archetypeId, ArraySpanToFragments( fragments ), ArraySpanToTags( tags ) );
}

void ecs::ECSManager::RemoveFragmentsAndTagsFromArchetype( const ecs::ArchetypeID& archetypeId, FragmentsFlags fragmentsToRemove, TagsFlags tagsToRemove )
{
	SetArchetypeFragmentsAndTags( archetypeId, archetypeId.GetFragmentsFlags() & fragmentsToRemove.Flipped(), archetypeId.GetTagsFlags() & tagsToRemove.Flipped() );
}

ecs::Archetype* ecs::ECSManager::GetEntityArchetype( EntityID id )
{
	auto it = m_entityToArchetype.find( id );
	if ( it != m_entityToArchetype.end() )
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

void ecs::ECSManager::VisitAllArchetypes( std::function< void( ecs::ArchetypeView ) > visitFunc )
{
	for ( auto& archetype : m_archetypes )
	{
		visitFunc( { *archetype } );
	}
}
