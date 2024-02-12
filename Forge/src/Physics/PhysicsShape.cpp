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
	physx::PxTriangleMeshCookingResult::Enum result = physx::PxTriangleMeshCookingResult::eFAILURE;
	FORGE_ASSURE( proxy.GetCooking().cookTriangleMesh( meshDesc, writeBuffer, &result ) );
	FORGE_ASSERT( result != physx::PxTriangleMeshCookingResult::eFAILURE );

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

void physics::PhysicsShape::ChangeScale( const Vector3& prevScale, const Vector3& newScale )
{
	ChangeScale( GetShape(), prevScale, newScale );
}

void physics::PhysicsShape::ChangeScale( physx::PxShape& shape, const Vector3& prevScale, const Vector3& newScale )
{
	if ( prevScale == newScale )
	{
		return;
	}

	physx::PxGeometryHolder holder( shape.getGeometry() );
	switch ( holder.getType() )
	{
	case physx::PxGeometryType::eSPHERE:
	{
		Float newScaleUnified = newScale.X;

		if ( newScale.X != newScale.Y || newScale.Y != newScale.Z )
		{
			FORGE_LOG_WARNING( "Non-uniform scale is not supported by sphere collider. Rounding to the highest component." );
			newScaleUnified = Math::Max( newScale.X, newScale.Y, newScale.Z );
		}

		holder.sphere().radius *= newScale.X / Math::Max( prevScale.X, prevScale.Y, prevScale.Z );
		break;
	}
	case physx::PxGeometryType::eCAPSULE:
	{
		holder.capsule().halfHeight *= newScale.Z / prevScale.Z;

		Float newScaleXUnified = newScale.X;

		if ( newScale.X != newScale.Y )
		{
			FORGE_LOG_WARNING( "Non-uniform scale X and Y is not supported by capsule collider. Rounding to the highest component." );
			newScaleXUnified = Math::Max( newScale.X, newScale.Y );
		}

		holder.capsule().radius *= newScaleXUnified / Math::Max( prevScale.X, prevScale.Y );
		break;
	}
	case physx::PxGeometryType::eBOX:
		holder.box().halfExtents.x *= newScale.X / prevScale.X;
		holder.box().halfExtents.y *= newScale.Y / prevScale.Y;
		holder.box().halfExtents.z *= newScale.Z / prevScale.Z;
		break;
	case physx::PxGeometryType::eCONVEXMESH:
		holder.convexMesh().scale.scale.x *= newScale.X / prevScale.X;
		holder.convexMesh().scale.scale.y *= newScale.Y / prevScale.Y;
		holder.convexMesh().scale.scale.z *= newScale.Z / prevScale.Z;
		break;
	case physx::PxGeometryType::eTRIANGLEMESH:
		holder.triangleMesh().scale.scale.x *= newScale.X / prevScale.X;
		holder.triangleMesh().scale.scale.y *= newScale.Y / prevScale.Y;
		holder.triangleMesh().scale.scale.z *= newScale.Z / prevScale.Z;
		break;
	default:
		FORGE_ASSERT( false );
		break;
	}

	shape.setGeometry( holder.any() );
}
