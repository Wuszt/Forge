#include "Fpch.h"
#include "PhysicsScene.h"
#include "../../External/physx/include/PxPhysicsAPI.h"
#include "PhysxProxy.h"
#include "PhysicsActor.h"

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

void physics::PhysicsScene::Simulate( Float deltaTime )
{
	if ( deltaTime > 0.5f )
	{
		return;
	}

	m_pxScene->simulate( deltaTime );
	m_pxScene->fetchResults( true );
}
