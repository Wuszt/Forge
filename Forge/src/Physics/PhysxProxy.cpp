#include "Fpch.h"
#include "PhysxProxy.h"

#include "../../External/physx/include/PxPhysicsAPI.h"
#include "PhysxScene.h"

#pragma comment( lib, "../../External/physx/lib/debug/PhysX_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXCommon_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXCooking_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXFoundation_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXPvdSDK_static_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXVehicle_static_64.lib" )

namespace
{
	const Float c_defaultStaticFriction = 0.5f;
	const Float c_defaultDynamicFriction = 0.5f;
	const Float c_defaultRestitution = 0.5f;
	const Float c_defaultDensity = 10.f;
}

physics::PhysxProxy::PhysxProxy()
	: m_pxAllocator( std::make_unique< physx::PxDefaultAllocator >() )
	, m_pxErrorCallback( std::make_unique< physx::PxDefaultErrorCallback >() )
{
	m_pxFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, *m_pxAllocator, *m_pxErrorCallback );

	m_pvd = physx::PxCreatePvd( *m_pxFoundation );

	m_transport = physx::PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10u );
	m_pvd->connect( *m_transport, physx::PxPvdInstrumentationFlag::eALL );

	m_physX = PxCreatePhysics( PX_PHYSICS_VERSION, *m_pxFoundation, physx::PxTolerancesScale(), true, m_pvd );
	PxInitExtensions( *m_physX, m_pvd );

	m_pxDispatcher = physx::PxDefaultCpuDispatcherCreate( 2u );

	m_pxDefaultMaterial = m_physX->createMaterial( c_defaultStaticFriction, c_defaultDynamicFriction, c_defaultRestitution );
}

physics::PhysxProxy::~PhysxProxy()
{
	m_pxDefaultMaterial->release();
	m_pxDispatcher->release();

	PxCloseExtensions();
	m_physX->release();
	m_transport->release();
	m_pvd->release();
	m_pxFoundation->release();
}

std::unique_ptr< physics::PhysxScene > physics::PhysxProxy::CreateScene()
{
	return std::make_unique< physics::PhysxScene >( m_physX, m_pxDispatcher );
}
