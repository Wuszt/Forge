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

		ArchetypeView archetype = m_ecsManager.GetEntityArchetype( entityID );
		const ArchetypeID originalID = archetype.GetArchetypeID();
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

	m_entitiesQueue.clear();

	for ( const auto& [ archetypeId, commands ] : m_archetypesQueue )
	{
		FragmentsFlags fragments = ( archetypeId.GetFragmentsFlags() | commands.m_fragmentsToAdd ) & commands.m_fragmentsToRemove.Flipped();
		TagsFlags tags = ( archetypeId.GetTagsFlags() | commands.m_tagsToAdd ) & commands.m_tagsToRemove.Flipped();

		m_ecsManager.SetArchetypeFragmentsAndTags( archetypeId, fragments, tags );
	}

	m_archetypesQueue.clear();
}
