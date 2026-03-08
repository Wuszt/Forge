#include "Fpch.h"
#include "PhysicsScene.h"
#include "../../External/physx/include/PxPhysicsAPI.h"
#include "PhysxProxy.h"
#include "PhysicsActor.h"
#include "RaycastResult.h"

physics::PhysicsScene::PhysicsScene( PhysxProxy& proxy )
{
	physx::PxSceneDesc sceneDesc( proxy.GetPhysics().getTolerancesScale() );
	sceneDesc.gravity = physx::PxVec3( 0.0f, 0.0f, -9.81f );

	sceneDesc.cpuDispatcher = &proxy.GetCPUDispatcher();
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	m_pxScene = proxy.GetPhysics().createScene( sceneDesc );

	if ( physx::PxPvdSceneClient* pvdClient = m_pxScene->getScenePvdClient() )
	{
		pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );
		pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );
		pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
	}
}

physics::PhysicsScene::~PhysicsScene()
{
	m_pxScene->release();
}

void physics::PhysicsScene::AddActor( physics::PhysicsActor& actor )
{
	m_pxScene->addActor( actor.GetActor() );
}

void physics::PhysicsScene::RemoveActor( physics::PhysicsActor& actor )
{
	m_pxScene->removeActor( actor.GetActor() );
}

bool physics::PhysicsScene::PerformRaycast( const Vector3& start, const Vector3& direction, Float length, Uint32 flags, physics::RaycastResult& outResult )
{
	physx::PxRaycastBuffer hitBuffer;
	physx::PxQueryFilterData filterData;
	filterData.data.word0 = flags;

	const bool anyHit = m_pxScene->raycast( physics::helpers::Convert( start ), physics::helpers::Convert( direction ), length, hitBuffer, physx::PxHitFlag::eDEFAULT, filterData );
	
	if ( anyHit )
	{
		outResult = physics::RaycastResult( hitBuffer.block );
	}

	return anyHit;
}

bool physics::PhysicsScene::PerformSphereOverlap( const Vector3& position, Float radius, Uint32 flags )
{
	const physx::PxSphereGeometry sphere( radius );
	physx::PxQueryFilterData filterData;
	filterData.data.word0 = flags;
	filterData.flags |= physx::PxQueryFlag::eANY_HIT;

	physx::PxOverlapBuffer overlapBuffer;

	physx::PxTransform transform( physics::helpers::Convert( position ) );

	const bool anyOverlap = m_pxScene->overlap( sphere, transform, overlapBuffer, filterData );
	return anyOverlap;
}

bool physics::PhysicsScene::PerformCubeOverlap( const Transform& transform, const Vector3& halfExtents, Uint32 flags )
{
	const physx::PxBoxGeometry box( physics::helpers::Convert( halfExtents ) );
	physx::PxQueryFilterData filterData;
	filterData.data.word0 = flags;
	filterData.flags |= physx::PxQueryFlag::eANY_HIT;

	physx::PxOverlapBuffer overlapBuffer;

	const bool anyOverlap = m_pxScene->overlap( box, physics::helpers::Convert( transform ), overlapBuffer, filterData );
	return anyOverlap;
}

bool physics::PhysicsScene::PerformCapsuleOverlap( const Transform& transform, Float radius, Float height, Uint32 flags )
{
	FORGE_ASSERT( height >= 2.0f * radius );

	const physx::PxCapsuleGeometry capsule( radius, height * 0.5f - radius );
	physx::PxQueryFilterData filterData;
	filterData.data.word0 = flags;
	filterData.flags |= physx::PxQueryFlag::eANY_HIT;

	physx::PxOverlapBuffer overlapBuffer;
	const bool anyOverlap = m_pxScene->overlap( capsule, physics::helpers::Convert( Transform( Quaternion( 0.0f, DEG2RAD * 90.0f, 0.0f ) ) * transform ), overlapBuffer, filterData,
		 nullptr, nullptr );

	return anyOverlap;
}

void physics::PhysicsScene::Simulate( Float deltaTime )
{
	constexpr Float minDelta = 0.02f;

	while ( deltaTime > 0.0f )
	{
		m_pxScene->simulate( Math::Min( deltaTime, minDelta ) );
		m_pxScene->fetchResults( true );

		deltaTime -= minDelta;
	}
}
