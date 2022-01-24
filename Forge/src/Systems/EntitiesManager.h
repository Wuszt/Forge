#pragma once

namespace forge
{
	class Entity;
	class EngineInstance;

	class EntitiesManager : public IManager
	{
		friend class Entity;
	public:
		using IManager::IManager;

		virtual void Initialize() override;
		virtual void Deinitialize() override;

		template< class T = forge::Entity >
		FORGE_INLINE void RequestCreatingEntity( const std::function< void( T* ) >& initializeFunc = nullptr )
		{
			EntityCreationRequest req;
			req.m_creationFunc = [ this, initializeFunc ]()
			{
				auto* rawEnt = CreateEntity< T >();

				if( initializeFunc )
				{
					initializeFunc( rawEnt );
				}
			};

			m_entityCreationRequests.emplace_back( req );
		}

		FORGE_INLINE void RequestAddingComponentsToEntity( const std::function< void() >& creationFunc )
		{
			ComponentCreationRequest req;
			req.m_creationFunc = creationFunc;

			m_componentCreationRequests.emplace_back( req );
		}

		FORGE_INLINE void RequestDestructingEntity( forge::EntityID id )
		{
			m_entityDestructionRequests.emplace_back( EntityDestructionRequest{ id } );
		}

		FORGE_INLINE Callback< EntityID >& GetEntityCreatedCallback()
		{
			return m_onEntityAdded;
		}

		FORGE_INLINE Callback< EntityID >& GetEntityDestructedCallback()
		{
			return m_onEntityDestructed;
		}

		FORGE_INLINE Uint32 GetEntitiesAmount() const
		{
			return static_cast< Uint32 >( m_entities.size() );
		}

		FORGE_INLINE Uint32 GetHighestID() const
		{
			return m_lastUsedEntityID;
		}

		template< class TEntity = Entity >
		FORGE_INLINE TEntity* GetEntity( const EntityID& id )
		{
			if( m_entities.count( id ) == 0u )
			{
				return nullptr;
			}

			return static_cast< TEntity* >( m_entities[ id ].get() );
		}

		void HandleRequests();

	private:
		template< class T = forge::Entity >
		FORGE_INLINE T* CreateEntity()
		{
			EntityID id = ++m_lastUsedEntityID;
			auto ent = std::make_unique< T >( GetEngineInstance(), id );
			auto* rawEnt = ent.get();

			m_entities.emplace( id, std::move( ent ) );

			m_onEntityAdded.Invoke( id );

			rawEnt->OnAttach();

			return rawEnt;
		}

		void RemoveEntity( const EntityID& id );

		Uint32 m_lastUsedEntityID = 0u;
		std::unordered_map< EntityID, std::unique_ptr< Entity > > m_entities;

		Callback< EntityID > m_onEntityAdded;
		Callback< EntityID > m_onEntityDestructed;

		struct EntityCreationRequest
		{
			std::function< void() > m_creationFunc;
		};
		std::vector< EntityCreationRequest > m_entityCreationRequests;

		struct EntityDestructionRequest
		{
			forge::EntityID m_id;
		};
		std::vector< EntityDestructionRequest > m_entityDestructionRequests;

		struct ComponentCreationRequest
		{
			std::function< void() > m_creationFunc;
		};
		std::vector< ComponentCreationRequest > m_componentCreationRequests;

		std::unique_ptr< CallbackToken > m_tickToken;

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_debugOverlayToken;
#endif
	};
}

