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

		Archetype& UpdateEntityArchetype( EntityID entityID, const ArchetypeID& newID );

		void AddFragmentToEntity( EntityID entityID, const ecs::Fragment::Type& type )
		{
			PC_SCOPE_FUNC();

			Archetype* currentArchetype = m_entityToArchetype[ entityID ];
			ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();
			id.AddFragment( type );

			Archetype& newArchetype = UpdateEntityArchetype( entityID, id );

			if ( !newArchetype.GetArchetypeID().ContainsFragment( type ) )
			{
				FORGE_ASSERT( newArchetype.GetEntitiesAmount() == 1u );
				newArchetype.AddFragmentType( type );
			}
		}

		template< class T >
		void AddFragmentToEntity( EntityID entityID )
		{
			AddFragmentToEntity( entityID, T::GetTypeStatic() );
		}

		template< class T >
		void AddTagToEntity( EntityID entityID )
		{
			PC_SCOPE_FUNC();

			Archetype* currentArchetype = m_entityToArchetype[ entityID ];

			if ( currentArchetype->GetArchetypeID().ContainsTag< T >() )
			{
				return;
			}

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
			PC_SCOPE_FUNC();

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

		Archetype* GetEntityArchetype( EntityID id );

		template< class T >
		void RemoveTagFromEntity( EntityID entityID )
		{
			PC_SCOPE_FUNC();

			Archetype* currentArchetype = m_entityToArchetype[ entityID ];

			if ( !currentArchetype->GetArchetypeID().ContainsTag< T >() )
			{
				return;
			}

			ArchetypeID id = currentArchetype ? currentArchetype->GetArchetypeID() : ArchetypeID();
			id.RemoveTag< T > ( );

			Archetype& newArchetype = UpdateEntityArchetype( entityID, id );

			if( newArchetype.GetArchetypeID().ContainsTag< T >() )
			{
				FORGE_ASSERT( newArchetype.GetEntitiesAmount() == 1u );
				newArchetype.RemoveTag< T >();
			}
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