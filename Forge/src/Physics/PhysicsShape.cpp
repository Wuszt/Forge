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

	m_shape = proxy.GetPhysics().createShape( physx::PxBoxGeometry( { cubeHalfExtents.X, cubeHalfExtents.Y, cubeHalfExtents.Z } ), material->GetPhysxMaterial() );
}

physics::PhysicsShape::PhysicsShape( PhysxProxy& proxy, forge::ArraySpan<Vector3> vertices, forge::ArraySpan<Uint32> indices, const physics::PhysicsMaterial* material /*= nullptr */ )
{
	if ( material == nullptr )
	{
		material = &proxy.GetDefaultMaterial();
	}

	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = vertices.GetSize();
	meshDesc.points.stride = sizeof( Vector3 );
	meshDesc.points.data = vertices.begin();
	meshDesc.triangles.count = indices.GetSize() / 3;
	meshDesc.triangles.stride = 3 * sizeof( physx::PxU32 );
	meshDesc.triangles.data = indices.begin();

	physx::PxDefaultMemoryOutputStream writeBuffer;
	physx::PxTriangleMeshCookingResult::Enum result;
	FORGE_ASSURE( proxy.GetCooking().cookTriangleMesh( meshDesc, writeBuffer, &result ) );

	physx::PxDefaultMemoryInputData readBuffer( writeBuffer.getData(), writeBuffer.getSize() );
	
	m_shape = proxy.GetPhysics().createShape( physx::PxTriangleMeshGeometry{ proxy.GetPhysics().createTriangleMesh( readBuffer ) }, material->GetPhysxMaterial(), true );
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