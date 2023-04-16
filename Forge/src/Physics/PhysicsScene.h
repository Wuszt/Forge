#pragma once

namespace physx
{
	class PxScene;
}

namespace physics
{
	class PhysxProxy;
	class PhysicsActor;

	class PhysicsScene
	{
	public:
		PhysicsScene( PhysxProxy& proxy );
		~PhysicsScene();

		void AddActor( PhysicsActor& actor );
		void RemoveActor( PhysicsActor& actor );

		void Simulate( Float deltaTime );

	private:
		physx::PxScene* m_pxScene;
	};
}

