#include "Fpch.h"
#include "PhysicsActor.h"
#include "../../External/physx/include/PxPhysicsAPI.h"
#include "PhysxProxy.h"
#include "PhysicsShape.h"
#include "../../External/physx/include/PxRigidActor.h"

static physx::PxTransform Convert( const Transform& transform )
{
	return{{ transform.GetPosition3().X, transform.GetPosition3().Y, transform.GetPosition3().Z },
		{ transform.GetOrientation().i, transform.GetOrientation().j, transform.GetOrientation().k, transform.GetOrientation().r }};
}

void physics::PhysicsActor::SetTransform( const Transform& transform )
{
	GetActor().setGlobalPose( Convert( transform ) );
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

physics::PhysicsDynamicActor::PhysicsDynamicActor( PhysicsDynamicActor&& other )
{
	m_actor = other.m_actor;
	other.m_actor = nullptr;
}

physics::PhysicsDynamicActor::~PhysicsDynamicActor()
{
	if( m_actor )
	{
		m_actor->release();
	}
}

void physics::PhysicsDynamicActor::Initialize( PhysxProxy& proxy, Transform transform /*= Transform() */ )
{
	m_actor = proxy.GetPhysics().createRigidDynamic( Convert( transform ) );
}

void physics::PhysicsDynamicActor::UpdateDensity( Float density )
{
	physx::PxRigidBodyExt::updateMassAndInertia( *m_actor, density );
}

const physx::PxRigidActor& physics::PhysicsDynamicActor::GetActor() const
{
	return *m_actor;
}

physx::PxRigidActor& physics::PhysicsDynamicActor::GetActor()
{
	return *m_actor;
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
	m_actor = proxy.GetPhysics().createRigidStatic( Convert( transform ) );
}

const physx::PxRigidActor& physics::PhysicsStaticActor::GetActor() const
{
	return *m_actor;
}

physx::PxRigidActor& physics::PhysicsStaticActor::GetActor()
{
	return *m_actor;
}
