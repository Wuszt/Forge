#pragma once

namespace physx
{
	class PxScene;
}

namespace physics
{
	class PhysxProxy;
	class PhysicsActor;
	struct RaycastResult;

	class PhysicsScene
	{
	public:
		PhysicsScene( PhysxProxy& proxy );
		~PhysicsScene();

		void AddActor( PhysicsActor& actor );
		void RemoveActor( PhysicsActor& actor );

		bool PerformRaycast( const Vector3& start, const Vector3& direction, Float length, Uint32 flags, physics::RaycastResult& outResult );

		void Simulate( Float deltaTime );

	private:
		physx::PxScene* m_pxScene;
	};
}

