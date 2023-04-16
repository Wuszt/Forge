#pragma once
#include "../GameEngine/ISystem.h"

namespace physics
{
	class PhysxProxy;
	class PhysicsScene;
	class PhysicsActor;
}

namespace systems
{
	class PhysicsSystem : public ISystem
	{
		DECLARE_POLYMORPHIC_CLASS( PhysicsSystem, systems::ISystem );

	public:
		PhysicsSystem();
		PhysicsSystem( PhysicsSystem&& );
		~PhysicsSystem();

		virtual void OnInitialize() override;
		virtual void OnDeinitialize() override;

		void RegisterActor( physics::PhysicsActor& actor );
		void UnregisterActor( physics::PhysicsActor& actor );

		physics::PhysxProxy& GetPhysicsProxy()
		{
			return *m_physicsProxy;
		}

	private:
		void Update();
		std::unique_ptr< physics::PhysxProxy > m_physicsProxy;
		std::unique_ptr< physics::PhysicsScene > m_scene;
		forge::CallbackToken m_updateToken;
	};
}