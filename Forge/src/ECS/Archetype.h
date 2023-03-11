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
		void AddTags( const TagsFlags& tags )
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
			m_tagsFlags.Set( type , false );
		}

		template< class T >
		void RemoveTag()
		{
			RemoveTag( T::GetTypeStatic() );
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

		Bool ContainsAllTagsAndFragments( const TagsFlags& tags, const FragmentsFlags& fragments ) const
		{
			return ContainsAllTags( tags ) && ContainsAllFragments( fragments );
		}

		Bool ContainsAnyTagsAndFragments( const TagsFlags& tags, const FragmentsFlags& fragments ) const
		{
			return ContainsAnyTags( tags ) || ContainsAnyFragments( fragments );
		}

		Bool ContainsAllTags( const TagsFlags& tags ) const
		{
			return ( m_tagsFlags & tags ) == tags;
		}

		Bool ContainsAnyTags( const TagsFlags& tags ) const
		{
			return ( m_tagsFlags & tags ) != 0;
		}

		Bool ContainsAllFragments( const FragmentsFlags& fragments ) const
		{
			return ( m_fragmentsFlags & fragments ) == fragments;
		}

		Bool ContainsAnyFragments( const FragmentsFlags& fragments ) const
		{
			return ( m_fragmentsFlags & fragments ) != 0;
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
		Archetype( Uint32 size = 0u, forge::ArraySpan< const ecs::Fragment::Type* > fragments = {}, const TagsFlags& tags = TagsFlags() )
			: m_sparseSet( size, c_invalidIndex )
		{
			for ( const auto* fragmentType : fragments )
			{
				AddFragmentType( *fragmentType );
			}

			AddTags( tags );
		}

		template< class T >
		forge::ArraySpan< const T > GetFragments() const
		{
			return m_fragments.at( &T::GetTypeStatic() ).GetFragments< T >();
		}

		template< class T >
		forge::ArraySpan< T > GetFragments()
		{
			return m_fragments.at( &T::GetTypeStatic() ).GetFragments< T >();
		}

		template< class T >
		const T* GetFragment( EntityID id ) const
		{
			if( !ContainsObject( id ) || !m_id.ContainsFragment< T >() )
			{
				return nullptr;
			}

			return &GetFragments< T >()[ GetFragmentIndex( id ) ];
		}

		template< class T >
		T* GetFragment( EntityID id )
		{
			return const_cast< T* >( static_cast<const Archetype*>( this )->GetFragment< T >( id ) );
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

		void AddFragmentType( const ecs::Fragment::Type& type )
		{
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

		void AddTags( const TagsFlags& tags )
		{
			m_id.AddTags( tags );
		}

		void AddTag( const ecs::Tag::Type& type )
		{
			m_id.AddTag( type );
		}

		template< class T >
		void AddTag()
		{
			m_id.AddTag< T >();
		}

		void RemoveTag( const ecs::Tag::Type& type )
		{
			m_id.RemoveTag( type );
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

		void StealEntityFrom( EntityID id, Archetype& source )
		{
			for( auto& fragmentsPackage : m_fragments )
			{
				if( source.ContainsFragment( *fragmentsPackage.first ) )
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

		void AddEntity( EntityID id )
		{
			for( auto& fragmentsPackage : m_fragments )
			{
				fragmentsPackage.second.AddEmptyFragment();
			}

			m_indexToEntity.emplace_back( id );
			m_sparseSet[ static_cast< Uint32 >( id ) ] = m_entitiesAmount++;
		}

		void RemoveEntity( EntityID id )
		{
			*std::find( m_sparseSet.begin(), m_sparseSet.end(), GetEntitiesAmount() - 1u ) = GetFragmentIndex( id );

			for( auto& fragmentsPackage : m_fragments )
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

		std::vector< const Fragment::Type* > GetFragmentsTypes() const
		{
			std::vector< const Fragment::Type* > types;
			types.reserve( m_fragments.size() );

			for ( const auto& pair : m_fragments )
			{
				types.emplace_back( pair.first );
			}

			return types;
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

		static const Uint32 c_invalidIndex = std::numeric_limits< Uint32 >::max();
		std::vector< EntityID > m_indexToEntity;
		std::unordered_map< const Fragment::Type*, FragmentsPackage > m_fragments;
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
