#pragma once

namespace physx
{
	struct PxRaycastHit;
}

namespace physics
{
	struct RaycastResult
	{
		Vector3 m_position;

		static RaycastResult Convert( const physx::PxRaycastHit& hit );
	};
}

