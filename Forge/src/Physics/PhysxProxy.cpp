#include "Fpch.h"
#include "PhysxProxy.h"

#include "../../External/physx/include/PxPhysicsAPI.h"
#include "PhysicsScene.h"
#include "PhysicsMaterial.h"

#ifdef FORGE_DEBUG_VERSION
#pragma comment( lib, "../../External/physx/lib/debug/PhysX_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXCommon_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXCooking_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXFoundation_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXPvdSDK_static_64.lib" )
#pragma comment( lib, "../../External/physx/lib/debug/PhysXVehicle_static_64.lib" )
#else
#pragma comment( lib, "../../External/physx/lib/release/PhysX_64.lib" )
#pragma comment( lib, "../../External/physx/lib/release/PhysXCommon_64.lib" )
#pragma comment( lib, "../../External/physx/lib/release/PhysXCooking_64.lib" )
#pragma comment( lib, "../../External/physx/lib/release/PhysXExtensions_static_64.lib" )
#pragma comment( lib, "../../External/physx/lib/release/PhysXFoundation_64.lib" )
#pragma comment( lib, "../../External/physx/lib/release/PhysXPvdSDK_static_64.lib" )
#pragma comment( lib, "../../External/physx/lib/release/PhysXVehicle_static_64.lib" )
#endif

namespace
{
	const Float c_defaultStaticFriction = 0.5f;
	const Float c_defaultDynamicFriction = 0.5f;
	const Float c_defaultRestitution = 0.5f;
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

	m_defaultMaterial = std::make_unique< physics::PhysicsMaterial >( *this, c_defaultDynamicFriction, c_defaultStaticFriction, c_defaultRestitution );

	m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pxFoundation, { physx::PxTolerancesScale() } );
}

physics::PhysxProxy::~PhysxProxy()
{
	m_cooking->release();
	m_defaultMaterial.reset();

	m_pxDispatcher->release();
	PxCloseExtensions();
	m_physX->release();
	m_pvd->release();
	m_transport->release();

	m_pxFoundation->release();
}
