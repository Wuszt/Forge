#pragma once
#include "../../External/physx/include/PxPhysicsAPI.h"

namespace physics
{
	namespace helpers
	{
		static physx::PxTransform Convert( const Transform& transform, Vector3 scale = Vector3::ONES() )
		{
			return{ { transform.GetPosition3().X, transform.GetPosition3().Y, transform.GetPosition3().Z },
				{ transform.GetOrientation().i, transform.GetOrientation().j, transform.GetOrientation().k, transform.GetOrientation().r } };
		}

		static physx::PxVec3 Convert( const Vector3& vec )
		{
			return { vec.X, vec.Y, vec.Z };
		}

		static Vector3 Convert( const physx::PxVec3& vec )
		{
			return { vec.x, vec.y, vec.z };
		}
	}
}