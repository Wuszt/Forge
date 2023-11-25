#pragma once
#include "FragmentsPackage.h"
#include "Tag.h"
#include "Fragment.h"
#include "EntityID.h"
#include "../Math/Math.h"
#include "FragmentViews.h"

namespace ecs
{
	class ArchetypeID
	{
	public:
		ArchetypeID() = default;
		ArchetypeID( FragmentsFlags fragments, TagsFlags tags )
			: m_fragmentsFlags( fragments )
			, m_tagsFlags( tags )
		{}

		void AddTags( TagsFlags tags )
		{
			m_tagsFlags = m_tagsFlags | tags;
		}

		void AddTag( const ecs::Tag::Type& type )
		{
			m_tagsFlags.Set( type, true );
		}

		template< class T >
		void AddTag()
		{
			AddTag( T::GetTypeStatic() );
		}

		void RemoveTag( const ecs::Tag::Type& type )
		{
			m_tagsFlags.Set( type, false );
		}

		template< class T >
		void RemoveTag()
		{
			RemoveTag( T::GetTypeStatic() );
		}

		void RemoveTags( TagsFlags tags )
		{
			m_tagsFlags = m_tagsFlags & tags.Flipped();
		}

		void ClearTags()
		{
			m_tagsFlags.Reset();
		}

		void AddFragments( FragmentsFlags fragments )
		{
			m_fragmentsFlags = m_fragmentsFlags | fragments;
		}

		void AddFragment( const ecs::Fragment::Type& type )
		{
			m_fragmentsFlags.Set( type, true );
		}

		template< class T >
		void AddFragment()
		{
			AddFragment( T::GetTypeStatic() );
		}

		void RemoveFragments( FragmentsFlags fragments )
		{
			m_fragmentsFlags = m_fragmentsFlags & fragments.Flipped();
		}

		void RemoveFragment( const ecs::Fragment::Type& type )
		{
			m_fragmentsFlags.Set( type, false );
		}

		template< class T >
		void RemoveFragment()
		{
			RemoveFragment( T::GetTypeStatic() );
		}

		Bool ContainsTag( const ecs::Tag::Type& type ) const
		{
			return m_tagsFlags.Test( type );
		}

		template< class T >
		Bool ContainsTag() const
		{
			return ContainsTag( T::GetTypeStatic() );
		}

		Bool ContainsFragment( const ecs::Fragment::Type& type ) const
		{
			return m_fragmentsFlags.Test( type );
		}

		template< class T >
		Bool ContainsFragment() const
		{
			return ContainsFragment( T::GetTypeStatic() );
		}

		Bool ContainsAllTagsAndFragments( TagsFlags tags, FragmentsFlags fragments ) const
		{
			return ContainsAllTags( tags ) && ContainsAllFragments( fragments );
		}

		Bool ContainsAnyTagsAndFragments( TagsFlags tags, FragmentsFlags fragments ) const
		{
			return ContainsAnyTags( tags ) || ContainsAnyFragments( fragments );
		}

		Bool ContainsAllTags( TagsFlags tags ) const
		{
			return ( m_tagsFlags & tags ) == tags;
		}

		Bool ContainsAnyTags( TagsFlags tags ) const
		{
			return ( m_tagsFlags & tags ) != 0;
		}

		Bool ContainsAllFragments( FragmentsFlags fragments ) const
		{
			return ( m_fragmentsFlags & fragments ) == fragments;
		}

		Bool ContainsAnyFragments( FragmentsFlags fragments ) const
		{
			return ( m_fragmentsFlags & fragments ) != 0;
		}

		Bool operator==( const ArchetypeID& id ) const
		{
			return id.m_tagsFlags == m_tagsFlags && id.m_fragmentsFlags == m_fragmentsFlags;
		}

		FragmentsFlags GetFragmentsFlags() const
		{
			return m_fragmentsFlags;
		}

		TagsFlags GetTagsFlags() const
		{
			return m_tagsFlags;
		}

	private:
		TagsFlags m_tagsFlags;
		FragmentsFlags m_fragmentsFlags;
	};

	class Archetype
	{
	public:
		Archetype( Uint32 size = 0u, const ArchetypeID& id = ArchetypeID() )
			: m_sparseSet( size, c_invalidIndex )
		{
			id.GetFragmentsFlags().VisitSetTypes( [ & ]( const ecs::Fragment::Type& fragment )
				{
					AddFragmentType( fragment );
				} );

			m_id = id;
		}

		template< class T >
		forge::ArraySpan< const T > GetFragments() const
		{
			return m_fragments.at( &T::GetTypeStatic() ).GetFragments< T >();
		}

		template< class T >
		forge::ArraySpan< T > GetMutableFragments()
		{
			return m_fragments.at( &T::GetTypeStatic() ).GetFragments< T >();
		}

		template< class T >
		const T* GetFragment( EntityID id ) const
		{
			if ( !ContainsEntity( id ) || !m_id.ContainsFragment< T >() )
			{
				return nullptr;
			}

			return &GetFragments< T >()[ GetFragmentIndex( id ) ];
		}

		template< class T >
		T* GetMutableFragment( EntityID id )
		{
			if ( !ContainsEntity( id ) || !m_id.ContainsFragment< T >() )
			{
				return nullptr;
			}

			return &GetMutableFragments< T >()[ GetFragmentIndex( id ) ];
		}

		void OnEntityCreated()
		{
			m_sparseSet.emplace_back( c_invalidIndex );
		}

		virtual void OnObjectDestructed( EntityID id )
		{
			FORGE_ASSERT( m_sparseSet[ static_cast< Uint32 >( id ) ] == c_invalidIndex ); // pls destroy components before you destroy their owner
		}

		Bool ContainsEntity( EntityID id ) const
		{
			return m_sparseSet.at( static_cast< Uint32 >( id ) ) != c_invalidIndex;
		}

		void AddFragmentType( const ecs::Fragment::Type& type )
		{
			FORGE_ASSERT( !IsInUse() );
			FORGE_ASSERT( m_fragments.count( &type ) == 0 );
			FORGE_ASSERT( !m_id.ContainsFragment( type ) );

			m_fragments.emplace( &type, FragmentsPackage( type, m_entitiesAmount ) );
			m_id.AddFragment( type );
		}

		template< class T >
		void AddFragmentType()
		{
			AddFragmentType( T::GetTypeStatic() );
		}

		void RemoveFragmentType( const ecs::Fragment::Type& type )
		{
			FORGE_ASSERT( !IsInUse() );
			FORGE_ASSERT( m_fragments.count( &type ) > 0 );
			FORGE_ASSERT( m_id.ContainsFragment( type ) );

			m_fragments.erase( &type );
			m_id.RemoveFragment( type );
		}

		template< class T >
		void RemoveFragmentType()
		{
			RemoveFragmentType( T::GetTypeStatic() );
		}

		void AddTags( TagsFlags tags )
		{
			FORGE_ASSERT( !IsInUse() );
			m_id.AddTags( tags );
		}

		void AddTag( const ecs::Tag::Type& type )
		{
			FORGE_ASSERT( !IsInUse() );
			m_id.AddTag( type );
		}

		template< class T >
		void AddTag()
		{
			FORGE_ASSERT( !IsInUse() );
			m_id.AddTag< T >();
		}

		void RemoveTag( const ecs::Tag::Type& type )
		{
			FORGE_ASSERT( !IsInUse() );
			m_id.RemoveTag( type );
		}

		template< class T >
		void RemoveTag()
		{
			FORGE_ASSERT( !IsInUse() );
			m_id.RemoveTag< T >();
		}

		void RemoveTags( TagsFlags tags )
		{
			FORGE_ASSERT( !IsInUse() );
			m_id.RemoveTags( tags );
		}

		void ClearTags()
		{
			FORGE_ASSERT( !IsInUse() );
			m_id.ClearTags();
		}

		Uint32 GetEntitiesAmount() const
		{
			return m_entitiesAmount;
		}

		Bool IsEmpty() const
		{
			return m_entitiesAmount == 0u;
		}

		void StealEntityFrom( EntityID id, Archetype& source )
		{
			FORGE_ASSERT( !IsInUse() );
			FORGE_ASSERT( !source.IsInUse() );
			for ( auto& fragmentsPackage : m_fragments )
			{
				if ( source.ContainsFragment( *fragmentsPackage.first ) )
				{
					fragmentsPackage.second.MoveFragment( source.GetFragmentIndex( id ), source.GetFragmentsPackage( *fragmentsPackage.first ) );
				}
				else
				{
					fragmentsPackage.second.AddEmptyFragment();
				}
			}

			m_indexToEntity.emplace_back( id );
			m_sparseSet[ static_cast< Uint32 >( id ) ] = m_entitiesAmount++;
			source.RemoveEntity( id );
		}

		void StealEntitiesFrom( Archetype& source )
		{
			FORGE_ASSERT( !IsInUse() );
			FORGE_ASSERT( !source.IsInUse() );

			for ( auto& fragmentsPackage : m_fragments )
			{
				if ( source.ContainsFragment( *fragmentsPackage.first ) )
				{
					for ( Uint32 i = 0u; i < source.GetEntitiesAmount(); ++i )
					{
						fragmentsPackage.second.MoveFragment( i, source.GetFragmentsPackage( *fragmentsPackage.first ) );
					}
				}
				else
				{
					fragmentsPackage.second.AddEmptyFragments( source.GetEntitiesAmount() );
				}
			}

			m_indexToEntity.reserve( m_indexToEntity.size() + source.GetEntitiesAmount() );
			for ( Uint32 i = 0u; i < source.GetEntitiesAmount(); ++i )
			{
				EntityID id = source.GetEntityIDWithIndex( i );
				m_indexToEntity.emplace_back( id );
				m_sparseSet[ static_cast< Uint32 >( id ) ] = m_entitiesAmount++;
			}

			source = Archetype();
		}

		void AddEntity( EntityID id )
		{
			FORGE_ASSERT( !IsInUse() );

			for ( auto& fragmentsPackage : m_fragments )
			{
				fragmentsPackage.second.AddEmptyFragment();
			}

			m_indexToEntity.emplace_back( id );
			m_sparseSet[ static_cast< Uint32 >( id ) ] = m_entitiesAmount++;
		}

		void RemoveEntity( EntityID id )
		{
			FORGE_ASSERT( !IsInUse() );

			*std::find( m_sparseSet.begin(), m_sparseSet.end(), GetEntitiesAmount() - 1u ) = GetFragmentIndex( id );

			for ( auto& fragmentsPackage : m_fragments )
			{
				fragmentsPackage.second.RemoveFragmentReorder( m_sparseSet[ static_cast< Uint32 >( id ) ] );
			}

			forge::utils::RemoveReorder( m_indexToEntity, m_sparseSet[ static_cast< Uint32 >( id ) ] );
			m_sparseSet[ static_cast< Uint32 >( id ) ] = c_invalidIndex;
			--m_entitiesAmount;
		}

		bool ContainsFragment( const Fragment::Type& type ) const
		{
			return m_fragments.find( &type ) != m_fragments.end();
		}

		EntityID GetEntityIDWithIndex( Uint32 index ) const
		{
			return m_indexToEntity[ index ];
		}

		Archetype GetEmptyCopy() const;

		const ArchetypeID& GetArchetypeID() const
		{
			return m_id;
		}

		void MarkAsInUse()
		{
			++m_inUseCounter;
		}

		void CancelInUse()
		{
			FORGE_ASSURE( m_inUseCounter-- > 0 );
		}

	private:
		Uint32 GetFragmentIndex( EntityID id ) const
		{
			return m_sparseSet[ static_cast< Uint32 >( id ) ];
		}

		const FragmentsPackage& GetFragmentsPackage( const Fragment::Type& type ) const
		{
			return m_fragments.at( &type );
		}

		FragmentsPackage& GetFragmentsPackage( const Fragment::Type& type )
		{
			return m_fragments.at( &type );
		}

		Bool IsInUse() const
		{
			return m_inUseCounter > 0;
		}

		static const Uint32 c_invalidIndex = std::numeric_limits< Uint32 >::max();
		std::vector< EntityID > m_indexToEntity;
		std::unordered_map< const Fragment::Type*, FragmentsPackage > m_fragments;
		std::vector< Uint32 > m_sparseSet;
		Uint32 m_entitiesAmount = 0u;
		ArchetypeID m_id;
		Uint32 m_inUseCounter = 0u;
	};

	class ArchetypeView
	{
	public:
		ArchetypeView( Archetype& archetype, FragmentsFlags readableFragments )
			: m_archetype( &archetype )
			, m_readableFragments( readableFragments )
		{
			m_archetype->MarkAsInUse();
		}

		ArchetypeView( const ArchetypeView& view ) : ArchetypeView( *view.m_archetype, view.m_readableFragments ) {}
		ArchetypeView( ArchetypeView&& view ) : ArchetypeView( *view.m_archetype, view.m_readableFragments ) 
		{
			view.Release();
		}

		~ArchetypeView()
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
		forge::ArraySpan< const T > GetFragments() const
		{
			FORGE_ASSERT( m_readableFragments.Test( T::GetTypeStatic() ) );
			return m_archetype->GetFragments< T >();
		}

		template< class T >
		const T* GetFragment( ecs::EntityID id ) const
		{
			FORGE_ASSERT( m_readableFragments.Test( T::GetTypeStatic() ) );
			return m_archetype->GetFragment< T >( id );
		}

		Uint32 GetEntitiesAmount() const
		{
			return m_archetype->GetEntitiesAmount();
		}

		EntityID GetEntityIDWithIndex( Uint32 index ) const
		{
			return m_archetype->GetEntityIDWithIndex( index );
		}

		const ArchetypeID& GetArchetypeID() const
		{
			return m_archetype->GetArchetypeID();
		}

		Bool ContainsEntity( EntityID id ) const
		{
			return m_archetype->ContainsEntity( id );
		}

	private:
		FragmentsFlags m_readableFragments;
		Archetype* m_archetype = nullptr;
	};

	class MutableArchetypeView
	{
	public:
		MutableArchetypeView( Archetype& archetype, FragmentsFlags mutableFragments, FragmentsFlags readableFragments )
			: m_archetype( &archetype )
			, m_mutableFragments( mutableFragments )
			, m_readableFragments( readableFragments )
		{
			m_archetype->MarkAsInUse();
		}

		MutableArchetypeView( const MutableArchetypeView& view ) : MutableArchetypeView( *view.m_archetype, view.m_mutableFragments, view.m_readableFragments ) {}
		MutableArchetypeView( MutableArchetypeView&& view ) : MutableArchetypeView( *view.m_archetype, view.m_mutableFragments, view.m_readableFragments ) 
		{
			view.Release();
		}

		~MutableArchetypeView()
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

		operator ArchetypeView() const
		{
			return ArchetypeView( *m_archetype, m_readableFragments );
		}

		template< class T >
		forge::ArraySpan< const T > GetFragments() const
		{
			FORGE_ASSERT( m_readableFragments.Test( T::GetTypeStatic() ) || m_mutableFragments.Test( T::GetTypeStatic() ) );
			return m_archetype->GetFragments< T >();
		}

		template< class T >
		forge::ArraySpan< T > GetMutableFragments() const
		{
			FORGE_ASSERT( m_mutableFragments.Test( T::GetTypeStatic() ) );
			return m_archetype->GetMutableFragments< T >();
		}

		template< class T >
		const T* GetFragment( ecs::EntityID id ) const
		{
			FORGE_ASSERT( m_readableFragments.Test( T::GetTypeStatic() ) || m_mutableFragments.Test( T::GetTypeStatic() ) );
			return m_archetype->GetFragment< T >( id );
		}

		template< class T >
		FragmentView< T > GetFragmentView( ecs::EntityID id ) const
		{
			FORGE_ASSERT( m_readableFragments.Test( T::GetTypeStatic() ) || m_mutableFragments.Test( T::GetTypeStatic() ) );
			return FragmentView< T >( *m_archetype, m_archetype->GetFragment< T >( id ) );
		}

		template< class T >
		T* GetMutableFragment( ecs::EntityID id ) const
		{
			FORGE_ASSERT( m_mutableFragments.Test( T::GetTypeStatic() ) );
			return m_archetype->GetMutableFragment< T >( id );
		}

		template< class T >
		MutableFragmentView< T > GetMutableFragmentView( ecs::EntityID id ) const
		{
			FORGE_ASSERT( m_mutableFragments.Test( T::GetTypeStatic() ) );
			return MutableFragmentView< T >( *m_archetype, m_archetype->GetMutableFragment< T >( id ) );
		}

		Uint32 GetEntitiesAmount() const
		{
			return m_archetype->GetEntitiesAmount();
		}

		EntityID GetEntityIDWithIndex( Uint32 index ) const
		{
			return m_archetype->GetEntityIDWithIndex( index );
		}

		const ArchetypeID& GetArchetypeID() const
		{
			return m_archetype->GetArchetypeID();
		}


		Bool ContainsEntity( EntityID id ) const
		{
			return m_archetype->ContainsEntity( id );
		}

	private:
		Archetype* m_archetype = nullptr;
		FragmentsFlags m_mutableFragments;
		FragmentsFlags m_readableFragments;
	};
}

namespace std
{
	template<>
	struct std::hash< ecs::ArchetypeID > {
		std::size_t operator()( const ecs::ArchetypeID& id ) const noexcept
		{
			return Math::CombineHashes( Math::CalculateHash( id.GetTagsFlags() ), Math::CalculateHash( id.GetFragmentsFlags() ) );
		}
	};
}
