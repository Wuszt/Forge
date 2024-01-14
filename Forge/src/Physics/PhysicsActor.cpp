#include "Fpch.h"
#include "PhysicsActor.h"
#include "../../External/physx/include/PxPhysicsAPI.h"
#include "PhysxProxy.h"
#include "PhysicsShape.h"
#include "../../External/physx/include/PxRigidActor.h"

RTTI_IMPLEMENT_TYPE( physics::PhysicsActor );
RTTI_IMPLEMENT_TYPE( physics::PhysicsDynamicActor );
RTTI_IMPLEMENT_TYPE( physics::PhysicsStaticActor );

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

void physics::PhysicsActor::Initialize( PhysxProxy& proxy, Uint32 group, Transform transform /*= Transform()*/, void* userData /*= nullptr */ )
{
	m_group = group;
	GetActor().userData = userData;
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

void physics::PhysicsActor::SetGroup( Uint32 group )
{
	m_group = group;
	std::vector< physx::PxShape* > shapes;
	shapes.resize( GetActor().getNbShapes() );
	GetActor().getShapes( shapes.data(), static_cast< Uint32 >( shapes.size() ) );

	physx::PxFilterData filterData;
	filterData.word0 = group;

	for ( physx::PxShape* shape : shapes )
	{
		shape->setQueryFilterData(filterData);
		shape->setSimulationFilterData(filterData);
	}
}

void physics::PhysicsActor::AddShape( physics::PhysicsShape&& shape )
{
	physx::PxFilterData filterData;
	filterData.word0 = m_group;

	shape.GetShape().setQueryFilterData( filterData );
	shape.GetShape().setSimulationFilterData( filterData );
	shape.ChangeScale( Vector3::ONES(), Vector3::ONES() * m_currentScale );
	GetActor().attachShape( shape.GetShape() );
}

void physics::PhysicsActor::ChangeScale( const Vector3& newScale )
{
	FORGE_ASSERT( Math::IsAlmostZero( newScale.X - newScale.Y, 0.0001f ) && Math::IsAlmostZero( newScale.Y - newScale.Z, 0.0001f ) );

	std::vector< physx::PxShape* > shapes;
	shapes.resize( GetActor().getNbShapes() );
	GetActor().getShapes( shapes.data(), static_cast< Uint32 >( shapes.size() ) );

	for ( auto* shape : shapes )
	{
		physics::PhysicsShape::ChangeScale( *shape, Vector3::ONES() * m_currentScale, newScale);
	}

	m_currentScale = newScale.X;
}

physics::PhysicsDynamicActor::PhysicsDynamicActor( PhysicsDynamicActor&& other ) : Super( std::move( other ) )
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

void physics::PhysicsDynamicActor::Initialize( PhysxProxy& proxy, Uint32 group, Transform transform /*= Transform() */, void* userData /*= nullptr */ )
{
	m_actor = proxy.GetPhysics().createRigidDynamic( physics::helpers::Convert( transform ) );
	Super::Initialize( proxy, group, transform, userData );
}

void physics::PhysicsDynamicActor::SetDensity( Float density )
{
	physx::PxRigidBodyExt::updateMassAndInertia( *m_actor, density * GetCurrentScale() );
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

void physics::PhysicsDynamicActor::ChangeScale( const Vector3& newScale )
{
	Super::ChangeScale( newScale );

	const Float scale = newScale.X / GetCurrentScale();
	const Float scale3 = scale * scale * scale;
	GetDynamicActor().setMass( GetDynamicActor().getMass() * scale3 );
	GetDynamicActor().setMassSpaceInertiaTensor( GetDynamicActor().getMassSpaceInertiaTensor() * scale3 );
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

physics::PhysicsStaticActor::PhysicsStaticActor( PhysicsStaticActor&& other ) : Super( std::move( other ) )
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

void physics::PhysicsStaticActor::Initialize( PhysxProxy& proxy, Uint32 group, Transform transform /*= Transform() */, void* userData /*= nullptr */ )
{
	m_actor = proxy.GetPhysics().createRigidStatic( physics::helpers::Convert( transform ) );
	Super::Initialize( proxy, group, transform, userData );
}

const physx::PxRigidActor& physics::PhysicsStaticActor::GetActor() const
{
	return GetStaticActor();
}

physx::PxRigidActor& physics::PhysicsStaticActor::GetActor()
{
	return GetStaticActor();
}
