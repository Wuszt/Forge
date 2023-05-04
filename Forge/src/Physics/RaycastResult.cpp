#include "Fpch.h"
#include "RaycastResult.h"

physics::RaycastResult physics::RaycastResult::Convert( const physx::PxRaycastHit& hit )
{
	RaycastResult result;
	result.m_position = physics::helpers::Convert( hit.position );

	return result;
}
