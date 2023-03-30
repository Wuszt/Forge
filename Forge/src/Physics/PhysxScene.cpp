#include "Fpch.h"
#include "PhysxScene.h"
#include "../../External/physx/include/PxPhysicsAPI.h"

physics::PhysxScene::PhysxScene( physx::PxPhysics* physX, physx::PxDefaultCpuDispatcher* dispatcher )
{
	physx::PxSceneDesc sceneDesc( physX->getTolerancesScale() );
	sceneDesc.gravity = physx::PxVec3( 0.0f, 0.0f, -9.81f );

	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	if ( physx::PxPvdSceneClient* pvdClient = m_pxScene->getScenePvdClient() )
	{
		pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );
		pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );
		pvdClient->setScenePvdFlag( physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
	}

	m_pxScene = physX->createScene( sceneDesc );
}

physics::PhysxScene::~PhysxScene()
{
	m_pxScene->release();
}
