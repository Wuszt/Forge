#pragma once
#include "../GameEngine/ISystem.h"

namespace physics
{
	class PhysxProxy;
	class PhysicsScene;
	class PhysicsActor;
	struct RaycastResult;
}

namespace systems
{
	class PhysicsSystem : public ISystem
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PhysicsSystem, systems::ISystem );

	public:
		PhysicsSystem();
		PhysicsSystem( PhysicsSystem&& );
		~PhysicsSystem();

		virtual void OnInitialize() override;
		virtual void OnDeinitialize() override;

		void RegisterActor( physics::PhysicsActor& actor );
		void UnregisterActor( physics::PhysicsActor& actor );

		bool PerformRaycast( const Vector3& start, const Vector3& direction, Float length, physics::RaycastResult& outResult );

		physics::PhysxProxy& GetPhysicsProxy()
		{
			return *m_physicsProxy;
		}

	private:
		void UpdateSimulation();
		void UpdateScene();
		std::unique_ptr< physics::PhysxProxy > m_physicsProxy;
		std::unique_ptr< physics::PhysicsScene > m_scene;
		forge::CallbackToken m_updateToken;
	};
}

namespace physics
{
	struct UserData
	{
		ecs::EntityID entityId;
		forge::ObjectID objectId;

		static UserData GetFromRaycastResult( const RaycastResult& raycastResult );
	};
}