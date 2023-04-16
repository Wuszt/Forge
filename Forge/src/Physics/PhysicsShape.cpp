#include "Fpch.h"
#include "PhysicsShape.h"
#include "../../External/physx/include/PxPhysicsAPI.h"
#include "PhysxProxy.h"
#include "PhysicsMaterial.h"

physics::PhysicsShape::PhysicsShape( PhysxProxy& proxy, const Vector3& cubeHalfExtents, const physics::PhysicsMaterial* material )
{
	if ( material == nullptr )
	{
		material = &proxy.GetDefaultMaterial();
	}

	m_shape = proxy.GetPhysics().createShape( physx::PxBoxGeometry( { cubeHalfExtents.X, cubeHalfExtents.Y, cubeHalfExtents.Z} ), material->GetPhysxMaterial() );
}

physics::PhysicsShape::PhysicsShape( physx::PxShape& rawShape )
	: m_shape( &rawShape )
{}

physics::PhysicsShape::PhysicsShape( PhysxProxy& proxy, Float capsuleRadius, Float capsuleHalfHeight, const physics::PhysicsMaterial* material )
{
	if ( material == nullptr )
	{
		material = &proxy.GetDefaultMaterial();
	}

	m_shape = proxy.GetPhysics().createShape( physx::PxCapsuleGeometry( capsuleRadius, capsuleHalfHeight ), material->GetPhysxMaterial() );
}

physics::PhysicsShape::PhysicsShape( PhysxProxy& proxy, Float sphereRadius, const physics::PhysicsMaterial* material )
{
	if ( material == nullptr )
	{
		material = &proxy.GetDefaultMaterial();
	}

	m_shape = proxy.GetPhysics().createShape( physx::PxSphereGeometry( sphereRadius ), material->GetPhysxMaterial() );
}

physics::PhysicsShape::~PhysicsShape()
{
	m_shape->release();
}