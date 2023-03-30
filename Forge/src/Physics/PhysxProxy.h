#pragma once

namespace physx
{
	class PxDefaultAllocator;
	class PxDefaultErrorCallback;
	class PxFoundation;
	class PxPhysics;
	class PxDefaultCpuDispatcher;
	class PxScene;

	class PxMaterial;
	class PxPvd;
	class PxPvdTransport;
}

namespace physics
{
	class PhysxScene;

	class PhysxProxy
	{
	public:
		PhysxProxy();
		~PhysxProxy();

		std::unique_ptr< PhysxScene > CreateScene();

	private:
		std::unique_ptr< physx::PxDefaultAllocator > m_pxAllocator;
		std::unique_ptr< physx::PxDefaultErrorCallback > m_pxErrorCallback;

		physx::PxFoundation* m_pxFoundation;
		physx::PxPhysics* m_physX;

		physx::PxDefaultCpuDispatcher* m_pxDispatcher;

		physx::PxMaterial* m_pxDefaultMaterial;
		physx::PxPvd* m_pvd;
		physx::PxPvdTransport* m_transport;
	};
}

