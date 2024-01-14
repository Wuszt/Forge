#pragma once

namespace physx
{
	struct PxRaycastHit;
	class PxActor;
}

namespace physics
{
	struct RaycastResult
	{
		RaycastResult() = default;
		RaycastResult( const physx::PxRaycastHit& hit );

		Vector3 m_position;
		void* m_actorData = nullptr;
	};
}

