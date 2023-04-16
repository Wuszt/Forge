#include "Fpch.h"
#include "PhysicsMaterial.h"
#include "PhysxProxy.h"
#include "../../External/physx/include/PxPhysicsAPI.h"

physics::PhysicsMaterial::PhysicsMaterial( PhysxProxy& proxy, Float staticFriction, Float dynamicFriction, Float restitution )
{
	m_material = proxy.GetPhysics().createMaterial( staticFriction, dynamicFriction, restitution );
}

physics::PhysicsMaterial::~PhysicsMaterial()
{
	m_material->release();
}
