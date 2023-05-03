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
		PhysicsShape( PhysxProxy& proxy, forge::ArraySpan<Vector3> vertices, forge::ArraySpan<Uint32> indices, const physics::PhysicsMaterial* material = nullptr );
		PhysicsShape( physx::PxShape& rawShape );

		void ChangeScale( const Vector3& prevScale, const Vector3& newScale );

		~PhysicsShape();

		physx::PxShape& GetShape() const
		{
			return *m_shape;
		}

		static void ChangeScale( physx::PxShape& shape, const Vector3& prevScale, const Vector3& newScale );

	private:
		physx::PxShape* m_shape = nullptr;
	};
}

