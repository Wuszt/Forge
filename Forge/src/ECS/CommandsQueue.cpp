#include "Fpch.h"
#include "CommandsQueue.h"

void ecs::CommandsQueue::AddFragment( EntityID entityID, const ecs::Fragment::Type& type )
{
	m_queue[ entityID ].m_fragmentsToAdd.Set( type, true );
	m_queue[ entityID ].m_fragmentsToRemove.Set( type, false );
}

void ecs::CommandsQueue::AddTag( EntityID entityID, const ecs::Tag::Type& type )
{
	m_queue[ entityID ].m_tagsToAdd.Set( type, true );
	m_queue[ entityID ].m_tagsToRemove.Set( type, false );
}

void ecs::CommandsQueue::RemoveFragment( EntityID entityID, const ecs::Fragment::Type& type )
{
	m_queue[ entityID ].m_fragmentsToAdd.Set( type, false );
	m_queue[ entityID ].m_fragmentsToRemove.Set( type, true );
}

void ecs::CommandsQueue::RemoveTag( EntityID entityID, const ecs::Tag::Type& type )
{
	m_queue[ entityID ].m_tagsToAdd.Set( type, false );
	m_queue[ entityID ].m_tagsToRemove.Set( type, true );
}

void ecs::CommandsQueue::RemoveEntity( EntityID entityID )
{
	m_queue[ entityID ].m_remove = true;
}

void ecs::CommandsQueue::Execute( ECSManager& ecsManager )
{
	for ( auto& [ entityID, commands ] : m_queue )
	{
		if ( commands.m_remove )
		{
			ecsManager.RemoveEntity( entityID );
			continue;
		}

		const Archetype* archetype = ecsManager.GetEntityArchetype( entityID );
		const ArchetypeID originalID = archetype ? ecsManager.GetEntityArchetype( entityID )->GetArchetypeID() : ArchetypeID();
		ArchetypeID newID = originalID;

		newID.m_fragmentsFlags = newID.m_fragmentsFlags | commands.m_fragmentsToAdd;

		commands.m_fragmentsToRemove.Flip();
		newID.m_fragmentsFlags = newID.m_fragmentsFlags & commands.m_fragmentsToRemove;

		newID.m_tagsFlags = newID.m_tagsFlags | commands.m_tagsToAdd;

		commands.m_tagsToRemove.Flip();
		newID.m_tagsFlags = newID.m_tagsFlags & commands.m_tagsToRemove;

		if ( newID != originalID )
		{
			ecsManager.MoveEntityToNewArchetype( entityID, newID );
		}
	}

	m_queue.clear();
}
