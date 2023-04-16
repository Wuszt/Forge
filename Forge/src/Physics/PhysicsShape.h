#pragma once

namespace physx
{
	class PxShape;
}

namespace physics
{
	class PhysicsMaterial;
	class PhysxProxy;

	class PhysicsShape
	{
	public:
		PhysicsShape( PhysxProxy& proxy, const Vector3& cubeHalfExtents, const physics::PhysicsMaterial* material = nullptr );
		PhysicsShape( PhysxProxy& proxy, Float sphereRadius, const physics::PhysicsMaterial* material = nullptr );
		PhysicsShape( PhysxProxy& proxy, Float capsuleRadius, Float capsuleHalfHeight, const physics::PhysicsMaterial* material = nullptr );
		PhysicsShape( physx::PxShape& rawShape );

		~PhysicsShape();

		physx::PxShape& GetShape() const
		{
			return *m_shape;
		}
	private:
		physx::PxShape* m_shape = nullptr;
	};
}

