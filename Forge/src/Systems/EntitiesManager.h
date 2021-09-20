#pragma once
#include "GameInstance.h"

namespace forge
{
	class Entity;
	class GameInstance;

	class EntitiesManager : public IManager
	{
	public:
		using IManager::IManager;

		template< class T >
		FORGE_INLINE T* CreateEntity()
		{
			EntityID id = ++m_lastUsedEntityID;
			auto ent = std::make_unique< T >( *this, id );
			auto* rawEnt = ent.get();

			m_entities.emplace( id, std::move( ent ) );

			m_onEntityAdded.Invoke( id );

			rawEnt->OnAttach();

			return rawEnt;
		}

		void RemoveEntity( const EntityID& id );

		FORGE_INLINE Callback< EntityID >& GetEntityCreationCallback()
		{
			return m_onEntityAdded;
		}

		FORGE_INLINE Uint32 GetEntitiesAmount() const
		{
			return static_cast< Uint32 >( m_entities.size() );
		}

		template< class TEntity = Entity >
		FORGE_INLINE TEntity* GetEntity( const EntityID& id ) const
		{
			return static_cast< TEntity* >( m_entities[ id ].get() );
		}

	private:
		Uint32 m_lastUsedEntityID = 0u;
		std::unordered_map< EntityID, std::unique_ptr< Entity > > m_entities;

		Callback< EntityID > m_onEntityAdded;
	};
}

