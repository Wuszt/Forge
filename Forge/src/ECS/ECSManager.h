#pragma once
#include "FragmentsPackage.h"
#include "Archetype.h"
#include "FragmentViews.h"

namespace ecs
{
	class Query;
	class MutableArchetypeView;
	class CommandsQueue;

	class FragmentUntypedView
	{
	public:
		FragmentUntypedView( Archetype& archetype, const ecs::Fragment::Type& fragmentType, EntityID id )
			: m_archetype( &archetype )
			, m_fragmentType( &fragmentType )
			, m_id( id )
		{
			m_archetype->MarkAsInUse();
		}

		FragmentUntypedView( const FragmentUntypedView& view )
		{
			m_archetype = view.m_archetype;
			m_fragmentType = view.m_fragmentType;
			m_id = view.m_id;

			m_archetype->MarkAsInUse();
		}

		FragmentUntypedView( FragmentUntypedView&& view )
		{
			m_archetype = view.m_archetype;
			m_fragmentType = view.m_fragmentType;
			m_id = view.m_id;

			m_archetype->MarkAsInUse();

			if ( view.m_archetype )
			{
				view.Release();
			}
		}

		~FragmentUntypedView()
		{
			if ( m_archetype )
			{
				Release();
			}
		}

		void Release()
		{
			m_archetype->CancelInUse();
			m_archetype = nullptr;
		}

		template< class T >
		const T* GetFragment() const
		{
			return m_archetype->GetFragment< T >( m_id );
		}

		EntityID GetOwnerEntityId() const
		{
			return m_id;
		}

	protected:
		Archetype* m_archetype = nullptr;
		const ecs::Fragment::Type* m_fragmentType;
		EntityID m_id;
	};

	class MutableFragmentUntypedView : public FragmentUntypedView
	{
	public:
		using FragmentUntypedView::FragmentUntypedView;

		template< class T >
		T* GetMutableFragment() const
		{
			return m_archetype->GetMutableFragment< T >( m_id );
		}
	};

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
			*GetMutableFragmentView< T >( entityID ) = std::move( fragmentData );
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
		FragmentView< T > GetFragmentView( EntityID id )
		{
			{
				Archetype& archetype = *m_entityToArchetype.at( id );
				if ( archetype.GetFragment< T >( id ) )
				{
					m_onBeforeReadingFragmentCallbacks[ ecs::Fragment::GetTypeIndex( T::GetTypeStatic() ) ].Invoke( MutableFragmentUntypedView{ archetype, T::GetTypeStatic(), id } );
				}
			}

			{
				Archetype& archetype = *m_entityToArchetype.at( id );
				return FragmentView< T >( archetype, archetype.GetFragment< T >( id ) );
			}
		}

		template< class T >
		MutableFragmentView< T > GetMutableFragmentView( EntityID id )
		{
			{
				Archetype& archetype = *m_entityToArchetype.at( id );
				if ( archetype.GetMutableFragment< T >( id ) )
				{
					m_onBeforeModifyingFragmentCallbacks[ ecs::Fragment::GetTypeIndex( T::GetTypeStatic() ) ].Invoke( MutableFragmentUntypedView{ archetype, T::GetTypeStatic(), id } );
				}
			}

			{
				Archetype& archetype = *m_entityToArchetype.at( id );
				return MutableFragmentView< T >( archetype, archetype.GetMutableFragment< T >( id ) );
			}
		}

		void RemoveTagFromEntity( EntityID entityID, const ecs::Tag::Type& type );

		template< class T >
		void RemoveTagFromEntity( EntityID entityID )
		{
			RemoveTagFromEntity( entityID, T::GetTypeStatic() );
		}

		void VisitAllArchetypes( FragmentsFlags readableFragments, std::function< void( ecs::ArchetypeView ) > visitFunc );

		template< class T >
		[[nodiscard]] forge::CallbackToken RegisterReadFragmentObserver( std::function< void( MutableFragmentUntypedView ) > callback )
		{
			return m_onBeforeReadingFragmentCallbacks[ ecs::Fragment::GetTypeIndex( T::GetTypeStatic() ) ].AddListener( callback );
		}

		template< class T >
		[[nodiscard]] forge::CallbackToken RegisterModifyingFragmentObserver( std::function< void( MutableFragmentUntypedView ) > callback )
		{
			return m_onBeforeModifyingFragmentCallbacks[ ecs::Fragment::GetTypeIndex( T::GetTypeStatic() ) ].AddListener( callback );
		}

		template< class T >
		[[nodiscard]] forge::CallbackToken RegisterReadArchetypeObserver( std::function< void( MutableArchetypeView, CommandsQueue& ) > callback, FragmentsFlags readableFragments, FragmentsFlags mutableFragments )
		{
			return m_onBeforeReadingArchetypeCallbacks[ ecs::Fragment::GetTypeIndex( T::GetTypeStatic() ) ].AddListener(
				[ callback = std::move( callback ), readableFragments, mutableFragments ]( Archetype& archetype, CommandsQueue& commandsQueue )
				{
					callback( ecs::MutableArchetypeView( archetype, mutableFragments, readableFragments ), commandsQueue );
				} );
		}
		
		template< class T >
		[[nodiscard]] forge::CallbackToken RegisterModifyingArchetypeObserver( std::function< void( MutableArchetypeView, CommandsQueue& ) > callback, FragmentsFlags readableFragments, FragmentsFlags mutableFragments )
		{
			return m_onBeforeModifyingArchetypeCallbacks[ ecs::Fragment::GetTypeIndex( T::GetTypeStatic() ) ].AddListener(
				[ callback = std::move( callback ), readableFragments, mutableFragments ]( Archetype& archetype, CommandsQueue& commandsQueue )
				{
					callback( ecs::MutableArchetypeView( archetype, mutableFragments, readableFragments ), commandsQueue );
				} );
		}

	private:
		Bool TryToFindArchetypeIndex( ArchetypeID Id, Uint32& outIndex ) const;

		void TriggerOnBeforeReadingArchetype( ecs::Archetype& archetype, ecs::FragmentsFlags fragments );
		void TriggerOnBeforeModifyingArchetype( ecs::Archetype& archetype, ecs::FragmentsFlags fragments );

		std::unordered_map< EntityID, Archetype* > m_entityToArchetype;
		std::vector< std::unique_ptr< Archetype > > m_archetypes;
		Archetype* m_emptyArchetype = nullptr;
		Uint32 m_nextEntityID = 0u;

		std::array< forge::Callback< MutableFragmentUntypedView >, ecs::FragmentsFlags::GetSize() > m_onBeforeReadingFragmentCallbacks;
		std::array< forge::Callback< MutableFragmentUntypedView >, ecs::FragmentsFlags::GetSize() > m_onBeforeModifyingFragmentCallbacks;
		std::array< forge::Callback< Archetype&, CommandsQueue& >, ecs::FragmentsFlags::GetSize() > m_onBeforeReadingArchetypeCallbacks;
		std::array< forge::Callback< Archetype&, CommandsQueue& >, ecs::FragmentsFlags::GetSize() > m_onBeforeModifyingArchetypeCallbacks;
	};
}