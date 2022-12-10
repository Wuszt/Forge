#pragma once
#include "EngineInstance.h"

namespace ecs
{
	class EntityID;
}

namespace forge
{
	class Object;
	class EngineInstance;

	class ObjectsManager
	{
		friend class Object;
	public:
		ObjectsManager( EngineInstance& engineInstance, UpdateManager& updateManager, ecs::ECSManager& ecsManager );

		template< class T = forge::Object >
		void RequestCreatingObject( const std::function< void( T* ) >& initializeFunc = nullptr )
		{
			ObjectCreationRequest req;
			req.m_creationFunc = [ this, initializeFunc ]()
			{
				auto* rawEnt = CreateObject< T >();

				if( initializeFunc )
				{
					initializeFunc( rawEnt );
				}
			};

			m_objectCreationRequests.emplace_back( req );
		}

		void RequestAddingComponentsToObject( const std::function< void() >& creationFunc )
		{
			ComponentCreationRequest req;
			req.m_creationFunc = creationFunc;

			m_componentCreationRequests.emplace_back( req );
		}

		void RequestDestructingObject( forge::ObjectID id )
		{
			m_objectDestructionRequests.emplace_back( ObjectDestructionRequest{ id } );
		}

		Callback< ObjectID >& GetObjectCreatedCallback()
		{
			return m_onObjectAdded;
		}

		Callback< ObjectID >& GetObjectDestructedCallback()
		{
			return m_onObjectDestructed;
		}

		Uint32 GetObjectsAmount() const
		{
			return static_cast< Uint32 >( m_objects.size() );
		}

		ecs::EntityID GetOrCreateEntityId( ObjectID id );

		template< class TObject = Object >
		TObject* GetObject( const ObjectID& id )
		{
			if( m_objects.count( id ) == 0u )
			{
				return nullptr;
			}

			return static_cast< TObject* >( m_objects[ id ].get() );
		}

		void HandleRequests();

	private:
		template< class T = forge::Object >
		T* CreateObject()
		{
			ObjectID id = ObjectID( m_nextObjectID++ );
			auto obj = std::make_unique< T >( m_engineInstance, id );
			auto* rawObj = obj.get();

			m_objects.emplace( id, std::move( obj ) );

			m_onObjectAdded.Invoke( id );

			rawObj->OnAttach();

			return rawObj;
		}

		void RemoveObject( const ObjectID& id );

		Uint32 m_nextObjectID = 0u;
		std::unordered_map< ObjectID, std::unique_ptr< Object > > m_objects;
		std::unordered_map< ObjectID, ecs::EntityID > m_objectsToEntities;

		ecs::ECSManager& m_ecsManager;
		forge::EngineInstance& m_engineInstance;

		Callback< ObjectID > m_onObjectAdded;
		Callback< ObjectID > m_onObjectDestructed;

		struct ObjectCreationRequest
		{
			std::function< void() > m_creationFunc;
		};
		std::vector< ObjectCreationRequest > m_objectCreationRequests;

		struct ObjectDestructionRequest
		{
			forge::ObjectID m_id;
		};
		std::vector< ObjectDestructionRequest > m_objectDestructionRequests;

		struct ComponentCreationRequest
		{
			std::function< void() > m_creationFunc;
		};
		std::vector< ComponentCreationRequest > m_componentCreationRequests;

		forge::CallbackToken m_tickToken;

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_debugOverlayToken;
#endif
	};
}

