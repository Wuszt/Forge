#pragma once
#include "FragmentsPackage.h"
#include "Archetype.h"

namespace ecs
{
	class ECSManager
	{
	public:
		EntityID CreateEntity();

		void RemoveEntity( EntityID id );

		void AddFragmentsAndTagsToEntity( EntityID entityID, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags );

		void AddFragmentsAndTagsToEntity( EntityID entityID, FragmentsFlags fragments, TagsFlags tags );

		void AddFragmentToEntity( EntityID entityID, const ecs::Fragment::Type& fragment )
		{
			AddFragmentsAndTagsToEntity( entityID, { &fragment }, {});
		}

		template< class T >
		void AddFragmentToEntity( EntityID entityID )
		{
			AddFragmentToEntity( entityID, T::GetTypeStatic() );
		}

		void MoveEntityToNewArchetype( EntityID entityID, const ArchetypeID& newID );

		void AddTagToEntity( EntityID entityID, const ecs::Tag::Type& type )
		{
			AddFragmentsAndTagsToEntity( entityID, {}, { &type } );
		}

		template< class T >
		void AddTagToEntity( EntityID entityID )
		{
			AddTagToEntity( entityID, T::GetTypeStatic() );
		}

		void RemoveFragmentFromEntity( EntityID entityID, const ecs::Fragment::Type& type );

		template< class T >
		void RemoveFragmentFromEntity( EntityID entityID )
		{
			RemoveFragmentFromEntity( entityID, T::GetTypeStatic() );
		}

		Archetype* GetEntityArchetype( EntityID id );

		void RemoveTagFromEntity( EntityID entityID, const ecs::Tag::Type& type );

		template< class T >
		void RemoveTagFromEntity( EntityID entityID )
		{
			RemoveTagFromEntity( entityID, T::GetTypeStatic() );
		}

		Bool TryToFindArchetypeIndex( ArchetypeID Id, Uint32& outIndex ) const;

		forge::ArraySpan< std::unique_ptr< Archetype > > GetArchetypes()
		{
			return m_archetypes;
		}

	private:
		std::unordered_map< EntityID, Archetype* > m_entityToArchetype;
		std::vector< std::unique_ptr< Archetype > > m_archetypes;
		Uint32 m_nextEntityID = 0u;
	};
}