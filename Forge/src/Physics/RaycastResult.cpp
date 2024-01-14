#include "Fpch.h"
#include "RaycastResult.h"
#include "../../External/physx/include/PxActor.h"

physics::RaycastResult::RaycastResult( const physx::PxRaycastHit& hit )
{
	m_position = physics::helpers::Convert( hit.position );
	m_actorData = hit.actor->userData;
}
