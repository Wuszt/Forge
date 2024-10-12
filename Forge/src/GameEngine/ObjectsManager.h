#pragma once
#include "EngineInstance.h"
#include "../ECS/EntityID.h"
#include "ObjectID.h"
#include "Object.h"

namespace forge
{
	class Object;
	class EngineInstance;

	class ObjectsManager
	{
		friend class Object;
	public:
		ObjectsManager( EngineInstance& engineInstance, UpdateManager& updateManager, ecs::ECSManager& ecsManager );
		~ObjectsManager();

		struct InitializationParams
		{
			std::function< void( Object& ) > m_preInitFunc = []( Object& ) {};
			std::function< void( Object& ) > m_postInitFunc = []( Object& ) {};
		};

		template< class T = forge::Object >
		void RequestCreatingObject( InitializationParams initParams = {} )
		{
			static_assert( std::is_base_of_v< forge::Object, T > );
			RequestCreatingObject( T::GetTypeStatic(), std::move( initParams ) );
		}

		template< class TFunc = decltype( []( forge::Object* ){} ) >
		void RequestCreatingObject( const forge::Object::Type& objectType, InitializationParams initParams = {} )
		{
			ObjectCreationRequest req;
			req.m_creationFunc = [ this, &objectType, initParams = std::move( initParams ) ]()
				{
					auto& rawObj =  CreateObject( objectType );

					initParams.m_preInitFunc( rawObj );
					rawObj.OnInit();
					initParams.m_postInitFunc( rawObj );

					rawObj.PostInit();
				};

			m_objectCreationRequests.emplace_back( req );
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

		Bool HasEntity( ObjectID id ) const
		{
			return m_objectsToEntities.find( id ) != m_objectsToEntities.end();
		}

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
		T& CreateObject()
		{
			return static_cast< T& >( CreateObject( T::GetTypeStatic() ) );
		}

		forge::Object& CreateObject( const forge::Object::Type& objectType );

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

		forge::CallbackToken m_tickToken;

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_debugOverlayToken;
#endif
	};
}

