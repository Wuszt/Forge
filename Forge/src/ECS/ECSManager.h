#pragma once
#include "FragmentsPackage.h"
#include "Archetype.h"

namespace ecs
{
	class Query;
	class MutableArchetypeView;
	class CommandsQueue;

	class ECSManager
	{
		friend class Query;

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
			AddFragmentsAndTagsToEntity( entityID, forge::ArraySpan< const ecs::Fragment::Type* >{ &fragment }, {} );
		}

		void MoveEntityToNewArchetype( EntityID entityID, const ArchetypeID& newID );

		void AddTagToEntity( EntityID entityID, const ecs::Tag::Type& type )
		{
			AddFragmentsAndTagsToEntity( entityID, {}, forge::ArraySpan< const ecs::Tag::Type* >{ &type } );
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

		void SetArchetypeFragmentsAndTags( const ecs::ArchetypeID& archetypeId, FragmentsFlags fragments, TagsFlags tags );
		void AddFragmentsAndTagsToArchetype( const ecs::ArchetypeID& archetypeId, FragmentsFlags fragments, TagsFlags tags );
		void RemoveFragmentsAndTagsFromArchetype( const ecs::ArchetypeID& archetypeId, FragmentsFlags fragments, TagsFlags tags );

		ArchetypeID GetEntityArchetypeId( EntityID id );

		template< class T >
		const T* GetFragment( EntityID id )
		{
			m_onBeforeReadingFragmentCallbacks[ ecs::Fragment::GetTypeIndex( T::GetTypeStatic() ) ].Invoke( id );
			return m_entityToArchetype.at( id )->GetFragment< T >( id );
		}

		template< class T >
		T* GetMutableFragment( EntityID id )
		{
			m_onBeforeModifyingFragmentCallbacks[ ecs::Fragment::GetTypeIndex( T::GetTypeStatic() ) ].Invoke( id );
			return m_entityToArchetype.at( id )->GetMutableFragment< T >( id );
		}

		void RemoveTagFromEntity( EntityID entityID, const ecs::Tag::Type& type );

		template< class T >
		void RemoveTagFromEntity( EntityID entityID )
		{
			RemoveTagFromEntity( entityID, T::GetTypeStatic() );
		}

		void VisitAllArchetypes( FragmentsFlags readableFragments, std::function< void( ecs::ArchetypeView ) > visitFunc );

		[[nodiscard]] forge::CallbackToken RegisterReadFragmentObserver( const ecs::Fragment::Type& observedFragmentType, std::function< void( ecs::EntityID ) > callback );
		[[nodiscard]] forge::CallbackToken RegisterModifyingFragmentObserver( const ecs::Fragment::Type& observedFragmentType, std::function< void( ecs::EntityID ) > callback );

		[[nodiscard]] forge::CallbackToken RegisterReadArchetypeObserver( const ecs::Fragment::Type& observedFragmentType, std::function< void( const MutableArchetypeView&, CommandsQueue& ) > callback, FragmentsFlags readableFragments, FragmentsFlags mutableFragments );
		[[nodiscard]] forge::CallbackToken RegisterModifyingArchetypeObserver( const ecs::Fragment::Type& observedFragmentType, std::function< void( const MutableArchetypeView&, CommandsQueue& ) > callback, FragmentsFlags readableFragments, FragmentsFlags mutableFragments );

	private:
		Bool TryToFindArchetypeIndex( ArchetypeID Id, Uint32& outIndex ) const;

		void TriggerOnBeforeReadingArchetype( ecs::Archetype& archetype, ecs::FragmentsFlags fragments );
		void TriggerOnBeforeModifyingArchetype( ecs::Archetype& archetype, ecs::FragmentsFlags fragments );

		std::unordered_map< EntityID, Archetype* > m_entityToArchetype;
		std::vector< std::unique_ptr< Archetype > > m_archetypes;
		Archetype* m_emptyArchetype = nullptr;
		Uint32 m_nextEntityID = 0u;

		std::array< forge::Callback< ecs::EntityID >, ecs::FragmentsFlags::GetSize() > m_onBeforeReadingFragmentCallbacks;
		std::array< forge::Callback< ecs::EntityID >, ecs::FragmentsFlags::GetSize() > m_onBeforeModifyingFragmentCallbacks;
		std::array< forge::Callback< Archetype&, CommandsQueue& >, ecs::FragmentsFlags::GetSize() > m_onBeforeReadingArchetypeCallbacks;
		std::array< forge::Callback< Archetype&, CommandsQueue& >, ecs::FragmentsFlags::GetSize() > m_onBeforeModifyingArchetypeCallbacks;
	};
}