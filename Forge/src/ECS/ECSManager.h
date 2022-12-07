#pragma once
#include "FragmentsPackage.h"
#include "Archetype.h"

namespace ecs
{
	class ECSManager : public forge::IManager
	{
	public:
		using IManager::IManager;

		EntityID CreateEntity()
		{
			for( auto& archetype : m_archetypes )
			{
				archetype->OnEntityCreated();
			}
			return ecs::EntityID( m_nextEntityID++ );
		}

		void RemoveEntity( EntityID id )
		{
			auto it = m_entityToArchetype.find( id );
			if( it != m_entityToArchetype.end() )
			{
				it->second->RemoveEntity( id );
				if( it->second->IsEmpty() )
				{
					Uint32 currentArchetypeIndex = 0u;
					FORGE_ASSURE( TryToFindArchetypeIndex( it->second->GetArchetypeID(), currentArchetypeIndex ) );
					forge::utils::RemoveReorder( m_archetypes, currentArchetypeIndex );
				}

				m_entityToArchetype.erase( it );
			}
		}

		Archetype& UpdateEntityArchetype( EntityID entityID, const ArchetypeID& newID );

		template< class T >
		void AddFragmentToEntity( EntityID entityID )
		{
			Archetype* currentArchetype = m_entityToArchetype[ entityID ];
			ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();
			id.AddFragment< T >();

			Archetype& newArchetype = UpdateEntityArchetype( entityID, id );

			if( !newArchetype.GetArchetypeID().ContainsFragment< T >() )
			{
				FORGE_ASSERT( newArchetype.GetEntitiesAmount() == 1u );
				newArchetype.AddFragmentType< T >();
			}
		}

		template< class T >
		void AddTagToEntity( EntityID entityID )
		{
			Archetype* currentArchetype = m_entityToArchetype[ entityID ];
			ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();
			id.AddTag< T >();

			Archetype& newArchetype = UpdateEntityArchetype( entityID, id );

			if( !newArchetype.GetArchetypeID().ContainsTag< T >() )
			{
				FORGE_ASSERT( newArchetype.GetEntitiesAmount() == 1u );
				newArchetype.AddTag< T >();
			}
		}

		template< class T >
		void RemoveFragmentFromEntity( EntityID entityID )
		{
			Archetype* currentArchetype = m_entityToArchetype[ entityID ];
			ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();
			id.RemoveFragment< T >();

			Archetype& newArchetype = UpdateEntityArchetype( entityID, id );

			if( newArchetype.GetArchetypeID().ContainsFragment< T >() )
			{
				FORGE_ASSERT( newArchetype.GetEntitiesAmount() == 1u );
				newArchetype.RemoveFragmentType< T >();
			}
		}

		template< class T >
		void RemoveTagFromEntity( EntityID entityID )
		{
			Archetype* currentArchetype = m_entityToArchetype[ entityID ];
			ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();
			id.RemoveTag< T > ( );

			Archetype& newArchetype = UpdateEntityArchetype( entityID, id );

			if( newArchetype.GetArchetypeID().ContainsTag< T >() )
			{
				FORGE_ASSERT( newArchetype.GetEntitiesAmount() == 1u );
				newArchetype.RemoveTag< T >();
			}
		}

		Bool TryToFindArchetypeIndex( ArchetypeID Id, Uint32& outIndex )
		{
			auto it = std::find_if( m_archetypes.begin(), m_archetypes.end(), [ &Id ]( const std::unique_ptr< Archetype >& archetype )
			{
				return archetype->GetArchetypeID() == Id;
			});

			outIndex = static_cast< Uint32 >( it - m_archetypes.begin() );

			return it != m_archetypes.end();
		}

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