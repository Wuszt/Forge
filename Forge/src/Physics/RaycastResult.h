#pragma once

namespace physx
{
	struct PxRaycastHit;
}

namespace physics
{
	struct RaycastResult
	{
		RaycastResult() = default;
		RaycastResult( const physx::PxRaycastHit& hit );

		Vector3 m_position;
		void* m_userData = nullptr;
	};
}

