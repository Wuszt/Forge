#include "Fpch.h"
#include "PhysicsActor.h"
#include "../../External/physx/include/PxPhysicsAPI.h"
#include "PhysxProxy.h"
#include "PhysicsShape.h"
#include "../../External/physx/include/PxRigidActor.h"

IMPLEMENT_TYPE( physics::PhysicsActor );
IMPLEMENT_TYPE( physics::PhysicsDynamicActor );
IMPLEMENT_TYPE( physics::PhysicsStaticActor );

static physx::PxForceMode::Enum Convert( physics::PhysicsDynamicActor::ForceMode mode )
{
	switch ( mode )
	{
	case physics::PhysicsDynamicActor::ForceMode::Force:
		return physx::PxForceMode::eFORCE;
	case physics::PhysicsDynamicActor::ForceMode::Impulse:
		return physx::PxForceMode::eIMPULSE;
	case physics::PhysicsDynamicActor::ForceMode::VelocityChange:
		return physx::PxForceMode::eVELOCITY_CHANGE;
	case physics::PhysicsDynamicActor::ForceMode::Acceleration:
		return physx::PxForceMode::eACCELERATION;
	default:
		FORGE_ASSERT( false );
	}

	return physx::PxForceMode::eFORCE;
}

void physics::PhysicsActor::SetTransform( const Transform& transform )
{
	GetActor().setGlobalPose( physics::helpers::Convert( transform ) );
}

Transform physics::PhysicsActor::GetTransform() const
{
	auto pxTransform = GetActor().getGlobalPose();
	Vector3 pos = { pxTransform.p.x, pxTransform.p.y, pxTransform.p.z };
	Quaternion rot = { pxTransform.q.x, pxTransform.q.y, pxTransform.q.z, pxTransform.q.w };

	return { pos, rot };
}

void physics::PhysicsActor::AddShape( const physics::PhysicsShape& shape )
{
	GetActor().attachShape( shape.GetShape() );
}

void physics::PhysicsActor::ChangeScale( const Vector3& prevScale, const Vector3& newScale )
{
	std::vector< physx::PxShape* > shapes;
	shapes.resize( GetActor().getNbShapes() );
	GetActor().getShapes( shapes.data(), static_cast< Uint32 >( shapes.size() ) );

	for ( auto* shape : shapes )
	{
		physics::PhysicsShape::ChangeScale( *shape, prevScale, newScale );
	}
}

physics::PhysicsDynamicActor::PhysicsDynamicActor( PhysicsDynamicActor&& other )
{
	m_actor = other.m_actor;
	other.m_actor = nullptr;
}

physics::PhysicsDynamicActor::~PhysicsDynamicActor()
{
	if ( m_actor )
	{
		m_actor->release();
	}
}

void physics::PhysicsDynamicActor::Initialize( PhysxProxy& proxy, Transform transform /*= Transform() */ )
{
	m_actor = proxy.GetPhysics().createRigidDynamic( physics::helpers::Convert( transform ) );
}

void physics::PhysicsDynamicActor::UpdateDensity( Float density )
{
	physx::PxRigidBodyExt::updateMassAndInertia( *m_actor, density );
}

void physics::PhysicsDynamicActor::AddForce( const Vector3& force, ForceMode forceMode )
{
	GetDynamicActor().addForce( physics::helpers::Convert( force ), Convert( forceMode ) );
}

void physics::PhysicsDynamicActor::AddTorque( const Vector3& torque, ForceMode forceMode )
{
	GetDynamicActor().addTorque( physics::helpers::Convert( torque ), Convert( forceMode ) );
}

void physics::PhysicsDynamicActor::ClearForce( ForceMode forceMode )
{
	GetDynamicActor().clearForce( Convert( forceMode ) );
}

void physics::PhysicsDynamicActor::ClearTorque( ForceMode forceMode )
{
	GetDynamicActor().clearTorque( Convert( forceMode ) );
}

void physics::PhysicsDynamicActor::EnableGravity( bool enable )
{
	GetDynamicActor().setActorFlag( physx::PxActorFlag::eDISABLE_GRAVITY, !enable );
}

void physics::PhysicsDynamicActor::EnableSimulation( bool enable )
{
	GetDynamicActor().setActorFlag( physx::PxActorFlag::eDISABLE_SIMULATION, !enable );
}

Vector3 physics::PhysicsDynamicActor::GetVelocity() const
{
	auto pxVec = GetDynamicActor().getLinearVelocity();
	return { pxVec.x, pxVec.y, pxVec.z };
}

void physics::PhysicsDynamicActor::SetVelocity( const Vector3& velocity )
{
	GetDynamicActor().setLinearVelocity( physics::helpers::Convert( velocity ) );
}

Vector3 physics::PhysicsDynamicActor::GetAngularVelocity() const
{
	auto pxVec = GetDynamicActor().getAngularVelocity();
	return { pxVec.x, pxVec.y, pxVec.z };
}

void physics::PhysicsDynamicActor::SetAngularVelocity( const Vector3& velocity )
{
	GetDynamicActor().setAngularVelocity( physics::helpers::Convert( velocity ) );
}

void physics::PhysicsDynamicActor::ChangeScale( const Vector3& prevScale, const Vector3& newScale )
{
	Super::ChangeScale( prevScale, newScale );

	FORGE_ASSERT( prevScale.X == prevScale.Y && prevScale.Y == prevScale.Z );
	FORGE_ASSERT( newScale.X == newScale.Y && newScale.Y == newScale.Z );

	const Float scale = newScale.X / prevScale.X;
	const Float scale3 = scale * scale * scale;
	GetDynamicActor().setMass( GetDynamicActor().getMass() * scale3 );
	GetDynamicActor().setMassSpaceInertiaTensor( GetDynamicActor().getMassSpaceInertiaTensor() * scale3 * scale * scale );
	physx::PxTransform cMassTransform = GetDynamicActor().getCMassLocalPose();
	GetDynamicActor().setCMassLocalPose( physx::PxTransform( cMassTransform.p * scale, cMassTransform.q ) );
}

const physx::PxRigidActor& physics::PhysicsDynamicActor::GetActor() const
{
	return GetDynamicActor();
}

physx::PxRigidActor& physics::PhysicsDynamicActor::GetActor()
{
	return GetDynamicActor();
}

physics::PhysicsStaticActor::PhysicsStaticActor( PhysicsStaticActor&& other )
{
	m_actor = other.m_actor;
	other.m_actor = nullptr;
}

physics::PhysicsStaticActor::~PhysicsStaticActor()
{
	if ( m_actor )
	{
		m_actor->release();
	}
}

void physics::PhysicsStaticActor::Initialize( PhysxProxy& proxy, Transform transform /*= Transform() */ )
{
	m_actor = proxy.GetPhysics().createRigidStatic( physics::helpers::Convert( transform ) );
}

const physx::PxRigidActor& physics::PhysicsStaticActor::GetActor() const
{
	return GetStaticActor();
}

physx::PxRigidActor& physics::PhysicsStaticActor::GetActor()
{
	return GetStaticActor();
}
