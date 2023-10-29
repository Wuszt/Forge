#pragma once

namespace ecs
{
	class CommandsQueue
	{
	public:
		void AddFragment( EntityID entityID, const ecs::Fragment::Type& type );
		void AddTag( EntityID entityID, const ecs::Tag::Type& type );

		void RemoveFragment( EntityID entityID, const ecs::Fragment::Type& type );
		void RemoveTag( EntityID entityID, const ecs::Tag::Type& type );

		void RemoveEntity( EntityID entityID );

		void AddFragment( ArchetypeID archetypeId, const ecs::Fragment::Type& type );
		void AddTag( ArchetypeID archetypeId, const ecs::Tag::Type& type );

		void RemoveFragment( ArchetypeID archetypeId, const ecs::Fragment::Type& type );
		void RemoveTag( ArchetypeID archetypeId, const ecs::Tag::Type& type );

		void Execute( ECSManager& ecsManager );

	private:
		struct EntityCommands
		{
			FragmentsFlags m_fragmentsToAdd;
			FragmentsFlags m_fragmentsToRemove;

			TagsFlags m_tagsToAdd;
			TagsFlags m_tagsToRemove;

			Bool m_remove = false;
		};

		struct ArchetypeCommands
		{
			FragmentsFlags m_fragmentsToAdd;
			FragmentsFlags m_fragmentsToRemove;

			TagsFlags m_tagsToAdd;
			TagsFlags m_tagsToRemove;
		};

		std::unordered_map< EntityID, EntityCommands > m_entitiesQueue;
		std::unordered_map< ArchetypeID, ArchetypeCommands > m_archetypesQueue;
	};
}
