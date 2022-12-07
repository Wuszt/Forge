#pragma once
#include "FragmentsPackage.h"
#include "Tag.h"
#include "Fragment.h"
#include "EntityID.h"
#include "../Math/Math.h"

namespace ecs
{
	struct ArchetypeID
	{
		template< class T >
		void AddTag()
		{
			m_tagsFlags.set( T::GetTagIndex(), true );
		}

		template< class T >
		void RemoveTag()
		{
			m_tagsFlags.set( T::GetTagIndex(), false );
		}

		template< class T >
		void AddFragment()
		{
			m_fragmentsFlags.set( T::GetFragmentIndex(), true );
		}

		template< class T >
		void RemoveFragment()
		{
			m_fragmentsFlags.set( T::GetFragmentIndex(), false );
		}

		template< class T >
		Bool ContainsTag() const
		{
			return m_tagsFlags.test( T::GetTagIndex() );
		}

		template< class T >
		Bool ContainsFragment() const
		{
			return m_fragmentsFlags.test( T::GetFragmentIndex() );
		}

		Bool ContainsTagsAndFragments( const TagsFlags& tags, const FragmentsFlags& fragments ) const
		{
			return ( ( m_tagsFlags & tags ) == tags ) && ( ( m_fragmentsFlags & fragments ) == fragments );
		}

		Bool operator==( const ArchetypeID& id ) const
		{
			return id.m_tagsFlags == m_tagsFlags && id.m_fragmentsFlags == m_fragmentsFlags;
		}

		TagsFlags m_tagsFlags;
		FragmentsFlags m_fragmentsFlags;
	};

	class Archetype
	{
	public:
		Archetype( Uint32 size = 0u )
			: m_sparseSet( size, c_invalidIndex )
		{}

		template< class T >
		const forge::ArraySpan< T >& GetFragments()
		{
			FORGE_ASSERT( dynamic_cast< FragmentsPackage< T >* >( m_fragments.at( &T::GetTypeStatic() ).get() ) );
			return static_cast< FragmentsPackage< T >* >( m_fragments.at( &T::GetTypeStatic() ).get() )->GetFragments();
		}

		template< class T >
		const T& GetFragment( EntityID id ) const
		{
			return GetFragments< T >()[ GetFragmentIndex( id ) ];
		}

		template< class T >
		T& GetFragment( EntityID id )
		{
			return GetFragments< T >()[ GetFragmentIndex( id ) ];
		}

		void OnEntityCreated()
		{
			m_sparseSet.emplace_back( c_invalidIndex );
		}

		virtual void OnObjectDestructed( EntityID id )
		{
			FORGE_ASSERT( m_sparseSet[ static_cast< Uint32 >( id ) ] == c_invalidIndex ); // pls destroy components before you destroy their owner
		}

		Bool ContainsObject( EntityID id ) const
		{
			return m_sparseSet.at( static_cast< Uint32 >( id ) ) >= 0;
		}

		template< class T >
		void AddFragmentType()
		{
			FORGE_ASSERT( m_fragments.count( &T::GetTypeStatic() ) == 0 );
			FORGE_ASSERT( !m_id.ContainsFragment< T >() );

			m_fragments.emplace( &T::GetTypeStatic(), std::make_unique< FragmentsPackage< T > >( m_entitiesAmount ) );
			m_id.AddFragment< T >();
		}

		template< class T >
		void RemoveFragmentType()
		{
			FORGE_ASSERT( m_fragments.count( &T::GetTypeStatic() ) > 0 );
			FORGE_ASSERT( !m_id.ContainsFragment< T >()  );

			m_fragments.emplace( &T::GetTypeStatic(), std::make_unique< FragmentsPackage< T > >( m_entitiesAmount ) );
			m_id.RemoveFragment< T >();
		}

		template< class T >
		void AddTag()
		{
			m_id.AddTag< T >();
		}

		template< class T >
		void RemoveTag()
		{
			m_id.RemoveTag< T >();
		}

		Uint32 GetEntitiesAmount() const
		{
			return m_entitiesAmount;
		}

		Bool IsEmpty() const
		{
			return m_entitiesAmount == 0u;
		}

		void AddEntityCopy( EntityID id, const Archetype& source )
		{
			for( auto& fragmentsPackage : m_fragments )
			{
				fragmentsPackage.second->CopyFragment( source.GetFragmentIndex( id ), source.GetFragmentsPackage( *fragmentsPackage.first ) );
			}

			m_sparseSet[ static_cast< Uint32 >( id ) ] = m_entitiesAmount++;
		}

		void AddEntity( EntityID id )
		{
			for( auto& fragmentsPackage : m_fragments )
			{
				fragmentsPackage.second->AddEmptyFragment();
			}

			m_sparseSet[ static_cast< Uint32 >( id ) ] = m_entitiesAmount++;
		}

		void RemoveEntity( EntityID id )
		{
			*std::find( m_sparseSet.begin(), m_sparseSet.end(), GetEntitiesAmount() - 1u ) = GetFragmentIndex( id );

			for( auto& fragmentsPackage : m_fragments )
			{
				fragmentsPackage.second->RemoveFragmentReorder( m_sparseSet[ static_cast< Uint32 >( id ) ] );
			}

			m_sparseSet[ static_cast< Uint32 >( id ) ] = c_invalidIndex;
			--m_entitiesAmount;
		}

		Archetype GetEmptyCopy() const;

		const ArchetypeID& GetArchetypeID() const
		{
			return m_id;
		}

	private:
		Uint32 GetFragmentIndex( EntityID id ) const
		{
			return m_sparseSet[ static_cast< Uint32 >( id ) ];
		}

		const IFragmentsPackage& GetFragmentsPackage( const Fragment::Type& type ) const
		{
			return *m_fragments.at( &type );
		}

		static const Uint32 c_invalidIndex = std::numeric_limits< Uint32 >::max();
		std::unordered_map< const Fragment::Type*, std::unique_ptr< IFragmentsPackage > > m_fragments;
		std::vector< Uint32 > m_sparseSet;
		Uint32 m_entitiesAmount = 0u;
		ArchetypeID m_id;
	};
}

namespace std
{
	template<> 
	struct std::hash< ecs::ArchetypeID > {
		std::size_t operator()( const ecs::ArchetypeID& id ) const noexcept
		{
			return Math::CombineHashes( Math::CalculateHash( id.m_tagsFlags ), Math::CalculateHash( id.m_fragmentsFlags ) );
		}
	};
}
