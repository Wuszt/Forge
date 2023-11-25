#include "Fpch.h"
#include "CommandsQueue.h"

void ecs::CommandsQueue::AddFragment( EntityID entityID, const ecs::Fragment::Type& type )
{
	m_entitiesQueue[ entityID ].m_fragmentsToAdd.Set( type, true );
	m_entitiesQueue[ entityID ].m_fragmentsToRemove.Set( type, false );
}

void ecs::CommandsQueue::AddFragment( ArchetypeID archetypeId, const ecs::Fragment::Type& type )
{
	m_archetypesQueue[ archetypeId ].m_fragmentsToAdd.Set( type, true );
	m_archetypesQueue[ archetypeId ].m_fragmentsToRemove.Set( type, false );
}

void ecs::CommandsQueue::AddTag( EntityID entityID, const ecs::Tag::Type& type )
{
	m_entitiesQueue[ entityID ].m_tagsToAdd.Set( type, true );
	m_entitiesQueue[ entityID ].m_tagsToRemove.Set( type, false );
}

void ecs::CommandsQueue::AddTag( ArchetypeID archetypeId, const ecs::Tag::Type& type )
{
	m_archetypesQueue[ archetypeId ].m_tagsToAdd.Set( type, true );
	m_archetypesQueue[ archetypeId ].m_tagsToRemove.Set( type, false );
}

void ecs::CommandsQueue::RemoveFragment( EntityID entityID, const ecs::Fragment::Type& type )
{
	m_entitiesQueue[ entityID ].m_fragmentsToAdd.Set( type, false );
	m_entitiesQueue[ entityID ].m_fragmentsToRemove.Set( type, true );
}

void ecs::CommandsQueue::RemoveFragment( ArchetypeID archetypeId, const ecs::Fragment::Type& type )
{
	m_archetypesQueue[ archetypeId ].m_fragmentsToAdd.Set( type, false );
	m_archetypesQueue[ archetypeId ].m_fragmentsToRemove.Set( type, true );
}

void ecs::CommandsQueue::RemoveTag( EntityID entityID, const ecs::Tag::Type& type )
{
	m_entitiesQueue[ entityID ].m_tagsToAdd.Set( type, false );
	m_entitiesQueue[ entityID ].m_tagsToRemove.Set( type, true );
}

void ecs::CommandsQueue::RemoveTag( ArchetypeID archetypeId, const ecs::Tag::Type& type )
{
	m_archetypesQueue[ archetypeId ].m_tagsToAdd.Set( type, false );
	m_archetypesQueue[ archetypeId ].m_tagsToRemove.Set( type, true );
}

void ecs::CommandsQueue::RemoveEntity( EntityID entityID )
{
	m_entitiesQueue[ entityID ].m_remove = true;
}

void ecs::CommandsQueue::Execute()
{
	for ( const auto& [ entityID, commands ] : m_entitiesQueue )
	{
		if ( commands.m_remove )
		{
			m_ecsManager.RemoveEntity( entityID );
			continue;
		}

		const ArchetypeID originalID = m_ecsManager.GetEntityArchetypeId( entityID );
		ArchetypeID newID = originalID;

		newID.AddFragments( commands.m_fragmentsToAdd );
		newID.RemoveFragments( commands.m_fragmentsToRemove );

		newID.AddTags( commands.m_tagsToAdd );
		newID.RemoveTags( commands.m_tagsToRemove );

		if ( newID != originalID )
		{
			m_ecsManager.MoveEntityToNewArchetype( entityID, newID );
		}
	}

	for ( const auto& [ archetypeId, commands ] : m_archetypesQueue )
	{
		FragmentsFlags fragments = ( archetypeId.GetFragmentsFlags() | commands.m_fragmentsToAdd ) & commands.m_fragmentsToRemove.Flipped();
		TagsFlags tags = ( archetypeId.GetTagsFlags() | commands.m_tagsToAdd ) & commands.m_tagsToRemove.Flipped();

		m_ecsManager.SetArchetypeFragmentsAndTags( archetypeId, fragments, tags );
	}

	Reset();
}

void ecs::CommandsQueue::Reset()
{
	m_entitiesQueue.clear();
	m_archetypesQueue.clear();
	m_postExecutionCallback = forge::Callback<>();
}

forge::CallbackToken ecs::CommandsQueue::AddPostExecutionCallback( std::function< void() > callback )
{
	return m_postExecutionCallback.AddListener( std::move( callback ) );
}

void ecs::CommandsQueue::Merge( CommandsQueue&& other )
{
	for ( auto&& it : std::move( other.m_archetypesQueue ) )
	{
		auto found = m_archetypesQueue.find( it.first );
		if ( found != m_archetypesQueue.end() )
		{
			found->second.m_fragmentsToAdd |= it.second.m_fragmentsToAdd;
			found->second.m_fragmentsToRemove &= it.second.m_fragmentsToAdd.Flipped();

			found->second.m_fragmentsToRemove |= it.second.m_fragmentsToRemove;
			found->second.m_fragmentsToAdd &= it.second.m_fragmentsToRemove;

			found->second.m_tagsToAdd |= it.second.m_tagsToAdd;
			found->second.m_tagsToRemove &= it.second.m_tagsToAdd.Flipped();

			found->second.m_tagsToRemove |= it.second.m_tagsToRemove;
			found->second.m_tagsToAdd &= it.second.m_tagsToRemove;
		}
		else
		{
			m_archetypesQueue.emplace( it );
		}
	}

	for ( auto&& it : std::move( other.m_entitiesQueue ) )
	{
		auto found = m_entitiesQueue.find( it.first );
		if ( found != m_entitiesQueue.end() )
		{
			found->second.m_fragmentsToAdd |= it.second.m_fragmentsToAdd;
			found->second.m_fragmentsToRemove &= it.second.m_fragmentsToAdd.Flipped();

			found->second.m_fragmentsToRemove |= it.second.m_fragmentsToRemove;
			found->second.m_fragmentsToAdd &= it.second.m_fragmentsToRemove;

			found->second.m_tagsToAdd |= it.second.m_tagsToAdd;
			found->second.m_tagsToRemove &= it.second.m_tagsToAdd.Flipped();

			found->second.m_tagsToRemove |= it.second.m_tagsToRemove;
			found->second.m_tagsToAdd &= it.second.m_tagsToRemove;

			found->second.m_remove |= it.second.m_remove;
		}
		else
		{
			m_entitiesQueue.emplace( it );
		}
	}

	m_postExecutionCallback.AddCallback( std::move( other.m_postExecutionCallback ) );

	other.Reset();
}
