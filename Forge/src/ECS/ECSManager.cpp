#include "Fpch.h"
#include "ECSManager.h"

ecs::ECSManager::ECSManager()
{
	m_emptyArchetype = m_archetypes.emplace_back( std::make_unique< Archetype >( m_nextEntityID ) ).get();
}

ecs::EntityID ecs::ECSManager::CreateEntity()
{
	PC_SCOPE_FUNC();

	for ( auto& archetype : m_archetypes )
	{
		archetype->OnEntityCreated();
	}

	ecs::EntityID id = ecs::EntityID( m_nextEntityID++ );
	
	m_emptyArchetype->AddEntity( id );
	m_entityToArchetype.emplace( id, m_emptyArchetype );

	return id;
}

void ecs::ECSManager::RemoveEntity( EntityID id )
{
	PC_SCOPE_FUNC();

	FORGE_ASSERT( id.IsValid() );

	Archetype& archetype = *m_entityToArchetype.at( id );
	archetype.RemoveEntity( id );
	if ( archetype.IsEmpty() && m_emptyArchetype != &archetype )
	{
		Uint32 currentArchetypeIndex = 0u;
		FORGE_ASSURE( TryToFindArchetypeIndex( archetype.GetArchetypeID(), currentArchetypeIndex ) );
		forge::utils::RemoveReorder( m_archetypes, currentArchetypeIndex );
	}

	m_entityToArchetype.erase( id );
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
	FORGE_ASSERT( entityID.IsValid() );

	Archetype& currentArchetype = *m_entityToArchetype.at( entityID );

	if ( ( fragments.Any() && tags.Any() )
		|| ( currentArchetype.GetArchetypeID().ContainsAllTagsAndFragments( tags, fragments ) ) )
	{
		return;
	}

	ArchetypeID id = currentArchetype.GetArchetypeID();
	id.AddTags( tags );
	id.AddFragments( fragments );

	MoveEntityToNewArchetype( entityID, id );
}

void ecs::ECSManager::MoveEntityToNewArchetype( EntityID entityID, const ArchetypeID& newID )
{
	PC_SCOPE_FUNC();

	FORGE_ASSERT( entityID.IsValid() );

	Archetype& currentArchetype = *m_entityToArchetype.at( entityID );
	Uint32 archetypeIndex = 0u;
	Archetype* targetArchetype = nullptr;
	if ( TryToFindArchetypeIndex( newID, archetypeIndex ) )
	{
		targetArchetype = m_archetypes[ archetypeIndex ].get();
	}
	else
	{
		std::vector< const ecs::Fragment::Type* > fragments;
		newID.GetFragmentsFlags().VisitSetTypes( [ & ]( const ecs::Fragment::Type& type )
			{
				if ( !currentArchetype.ContainsFragment( type ) )
				{
					fragments.emplace_back( &type );
				}
			} );

		targetArchetype = m_archetypes.emplace_back( std::make_unique< Archetype >( m_nextEntityID, newID ) ).get();
	}

	targetArchetype->StealEntityFrom( entityID, currentArchetype );

	if ( currentArchetype.IsEmpty() && m_emptyArchetype != &currentArchetype )
	{
		Uint32 currentArchetypeIndex = 0u;
		FORGE_ASSURE( TryToFindArchetypeIndex( currentArchetype.GetArchetypeID(), currentArchetypeIndex ) );
		forge::utils::RemoveReorder( m_archetypes, currentArchetypeIndex );
	}

	m_entityToArchetype[ entityID ] = targetArchetype;
}

void ecs::ECSManager::RemoveFragmentFromEntity( EntityID entityID, const ecs::Fragment::Type& type )
{
	PC_SCOPE_FUNC();

	FORGE_ASSERT( entityID.IsValid() );

	Archetype* currentArchetype = m_entityToArchetype.at( entityID );
	ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();

	if ( !id.ContainsFragment( type ) )
	{
		return;
	}

	id.RemoveFragment( type );

	MoveEntityToNewArchetype( entityID, id );
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

		if ( m_archetypes[ oldArchetypeIndex ].get() != m_emptyArchetype )
		{
			forge::utils::RemoveReorder( m_archetypes, oldArchetypeIndex );
		}
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

void ecs::ECSManager::RemoveFragmentsAndTagsFromArchetype( const ecs::ArchetypeID& archetypeId, FragmentsFlags fragmentsToRemove, TagsFlags tagsToRemove )
{
	SetArchetypeFragmentsAndTags( archetypeId, archetypeId.GetFragmentsFlags() & fragmentsToRemove.Flipped(), archetypeId.GetTagsFlags() & tagsToRemove.Flipped() );
}

ecs::ArchetypeID ecs::ECSManager::GetEntityArchetypeId( EntityID id )
{
	FORGE_ASSERT( id.IsValid() );
	return m_entityToArchetype.at( id )->GetArchetypeID();
}

void ecs::ECSManager::RemoveTagFromEntity( EntityID entityID, const ecs::Tag::Type& type )
{
	PC_SCOPE_FUNC();

	FORGE_ASSERT( entityID.IsValid() );

	Archetype& currentArchetype = *m_entityToArchetype.at( entityID );
	ArchetypeID id = currentArchetype.GetArchetypeID();

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

void ecs::ECSManager::TriggerOnBeforeReadingArchetype( ecs::Archetype& archetype, ecs::FragmentsFlags fragments )
{
	ecs::CommandsQueue commandsQueue( *this );
	fragments.VisitSetTypes( [ & ]( Uint32 index )
		{
			m_onBeforeReadingArchetypeCallbacks[ index ].Invoke( archetype, commandsQueue );
		} );

	commandsQueue.Execute();
}

void ecs::ECSManager::TriggerOnBeforeModifyingArchetype( ecs::Archetype& archetype, ecs::FragmentsFlags fragments )
{
	ecs::CommandsQueue commandsQueue( *this );
	fragments.VisitSetTypes( [ & ]( Uint32 index )
		{
			m_onBeforeModifyingArchetypeCallbacks[ index ].Invoke( archetype, commandsQueue );
		} );

	commandsQueue.Execute();
}

void ecs::ECSManager::VisitAllArchetypes( FragmentsFlags readableFragments, std::function< void( ecs::ArchetypeView ) > visitFunc )
{
	for ( auto& archetype : m_archetypes )
	{
		visitFunc( { *archetype, readableFragments } );
	}
}

forge::CallbackToken ecs::ECSManager::RegisterReadFragmentObserver( const ecs::Fragment::Type& observedFragmentType, std::function< void( ecs::EntityID ) > callback )
{
	return m_onBeforeReadingFragmentCallbacks[ ecs::Fragment::GetTypeIndex( observedFragmentType ) ].AddListener( callback );
}

forge::CallbackToken ecs::ECSManager::RegisterModifyingFragmentObserver( const ecs::Fragment::Type& observedFragmentType, std::function< void( ecs::EntityID ) > callback )
{
	return m_onBeforeModifyingFragmentCallbacks[ ecs::Fragment::GetTypeIndex( observedFragmentType ) ].AddListener( callback );
}

forge::CallbackToken ecs::ECSManager::RegisterReadArchetypeObserver( const ecs::Fragment::Type& observedFragmentType, std::function< void( const MutableArchetypeView&, CommandsQueue& ) > callback, FragmentsFlags readableFragments, FragmentsFlags mutableFragments )
{
	return m_onBeforeReadingArchetypeCallbacks[ ecs::Fragment::GetTypeIndex( observedFragmentType ) ].AddListener(
		[ callback = std::move( callback ), readableFragments, mutableFragments ]( Archetype& archetype, CommandsQueue& commandsQueue )
		{
			callback( ecs::MutableArchetypeView( archetype, mutableFragments, readableFragments ), commandsQueue );
		});
}

forge::CallbackToken ecs::ECSManager::RegisterModifyingArchetypeObserver( const ecs::Fragment::Type& observedFragmentType, std::function< void( const MutableArchetypeView&, CommandsQueue& ) > callback, FragmentsFlags readableFragments, FragmentsFlags mutableFragments )
{
	return m_onBeforeModifyingArchetypeCallbacks[ ecs::Fragment::GetTypeIndex( observedFragmentType ) ].AddListener(
		[ callback = std::move( callback ), readableFragments, mutableFragments ]( Archetype& archetype, CommandsQueue& commandsQueue )
		{
			callback( ecs::MutableArchetypeView( archetype, mutableFragments, readableFragments ), commandsQueue );
		} );
}
