#pragma once
#include "FragmentsPackage.h"
#include <bitset>
#include "Tag.h"
#include "Fragment.h"
#include "EntityID.h"

namespace ecs
{
	class Archetype
	{
	public:
		Archetype( Uint32 size = 0u )
			: m_sparseSet( size + 1, -1 )
		{}

		template< class T >
		FragmentsPackage< T >& GetData()
		{
			FORGE_ASSERT( dynamic_cast< FragmentsPackage< T >* >( m_data.at( &T::GetTypeStatic() ).get() ) );
			return *static_cast< FragmentsPackage< T >* >( m_data.at( &T::GetTypeStatic() ).get() );
		}

		template< class T >
		const T& GetData( EntityID id ) const
		{
			Int32 index = m_sparseSet[ id ];
			return GetData< T >()[ index ];
		}

		template< class T >
		T& GetData( EntityID id )
		{
			Int32 index = m_sparseSet[ id ];
			return GetData< T >()[ index ];
		}

		IFragmentsPackage& GetData( const ecs::Fragment::Type& type )
		{
			return *m_data.at( &type );
		}

		void OnObjectCreated()
		{
			m_sparseSet.emplace_back( -1 );
		}

		virtual void OnObjectDestructed( EntityID id )
		{
			FORGE_ASSERT( m_sparseSet[ id ] == -1 ); // pls destroy components before you destroy their owner
		}

		Bool ContainsObject( EntityID id ) const
		{
			return m_sparseSet.at( id ) >= 0;
		}

		template< class T >
		Bool ContainsData() const
		{
			return ContainsData( T::GetTypeStatic() );
		}

		//It won't work since Fragment suppose to be non polymorphic type and by rtti doesn't allow inheritance without being polymorphic. I need to fix that
		Bool ContainsData( const ecs::Fragment::Type& type ) const
		{
			return m_data.count( &type ) > 0u;
		}

		template< class T >
		void AddDataPackage()
		{
			m_data.emplace( &T::GetTypeStatic(), std::make_unique< FragmentsPackage< T > >( m_dataSize ) );
		}

		void AddDataPackage( std::unique_ptr< IFragmentsPackage > package )
		{
			m_data.emplace( &package->GetDataType(), std::move( package ) );
		}

		Uint32 GetDataSize() const
		{
			return m_dataSize;
		}

		Bool IsEmpty() const
		{
			return GetDataSize() == 0u;
		}

		//void MoveObjectTo( forge::ObjectID objectId, Archetype* destination );
		//void MoveObjectTo( forge::ObjectID objectId, std::vector< std::unique_ptr< IFragmentsPackage > >& destination );
		//void MoveObjectFrom( forge::ObjectID objectId, std::vector< Archetype* > donorArchetypes );

	private:
		std::unordered_map< const Fragment::Type*, std::unique_ptr< IFragmentsPackage > > m_data;
		std::vector< Int32 > m_sparseSet;
		Uint32 m_dataSize = 0u;

		std::bitset< Tag::c_maxTagsAmount > m_tags;
		std::bitset< Fragment::c_maxFragmentsAmount > m_fragments;
	};
}