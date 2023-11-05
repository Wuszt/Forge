#pragma once
#include "FragmentsPackage.h"
#include "Archetype.h"

namespace ecs
{
	class ECSManager
	{
	public:
		ECSManager();

		EntityID CreateEntity();

		void RemoveEntity( EntityID id );

		void AddFragmentsAndTagsToEntity( EntityID entityID, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags );

		void AddFragmentsAndTagsToEntity( EntityID entityID, FragmentsFlags fragments, TagsFlags tags );

		template< class T >
		void AddFragmentToEntity( EntityID entityID )
		{
			AddFragmentToEntity( entityID, T::GetTypeStatic() );
		}

		template< class T >
		void AddFragmentDataToEntity( EntityID entityID, T fragmentData )
		{
			AddFragmentToEntity< T >( entityID );
			*GetMutableFragment< T >( entityID ) = std::move( fragmentData );
		}

		void AddFragmentToEntity( EntityID entityID, const ecs::Fragment::Type& fragment )
		{
			AddFragmentsAndTagsToEntity( entityID, { &fragment }, {} );
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

		void SetArchetypeFragmentsAndTags( const ecs::ArchetypeID& archetypeId, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags );
		void SetArchetypeFragmentsAndTags( const ecs::ArchetypeID& archetypeId, FragmentsFlags fragments, TagsFlags tags );

		void AddFragmentsAndTagsToArchetype( const ecs::ArchetypeID& archetypeId, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags );
		void AddFragmentsAndTagsToArchetype( const ecs::ArchetypeID& archetypeId, FragmentsFlags fragments, TagsFlags tags );

		void RemoveFragmentsAndTagsFromArchetype( const ecs::ArchetypeID& archetypeId, forge::ArraySpan< const ecs::Fragment::Type* > fragments, forge::ArraySpan< const ecs::Tag::Type* > tags );
		void RemoveFragmentsAndTagsFromArchetype( const ecs::ArchetypeID& archetypeId, FragmentsFlags fragments, TagsFlags tags );

		ArchetypeView GetEntityArchetype( EntityID id );

		template< class T >
		const T* GetFragment( EntityID id )
		{
			ArchetypeView archetype = GetEntityArchetype( id );
			return archetype.GetFragment< T >( id );
		}

		template< class T >
		T* GetMutableFragment( EntityID id )
		{
			ArchetypeView archetype = GetEntityArchetype( id );
			return archetype.GetMutableFragment< T >( id );
		}

		void RemoveTagFromEntity( EntityID entityID, const ecs::Tag::Type& type );

		template< class T >
		void RemoveTagFromEntity( EntityID entityID )
		{
			RemoveTagFromEntity( entityID, T::GetTypeStatic() );
		}

		void VisitAllArchetypes( std::function< void( ecs::ArchetypeView ) > visitFunc );

	private:
		Bool TryToFindArchetypeIndex( ArchetypeID Id, Uint32& outIndex ) const;

		std::unordered_map< EntityID, Archetype* > m_entityToArchetype;
		std::vector< std::unique_ptr< Archetype > > m_archetypes;
		Archetype* m_emptyArchetype = nullptr;
		Uint32 m_nextEntityID = 0u;
	};
}