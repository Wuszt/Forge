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
	class PhysicsScene;
	class PhysicsMaterial;

	class PhysxProxy
	{
	public:
		PhysxProxy();
		~PhysxProxy();

		physx::PxPhysics& GetPhysics() const
		{
			return *m_physX;
		}

		physx::PxDefaultCpuDispatcher& GetCPUDispatcher() const
		{
			return *m_pxDispatcher;
		}

		const PhysicsMaterial& GetDefaultMaterial() const
		{
			return *m_defaultMaterial;
		}

	private:
		std::unique_ptr< physx::PxDefaultAllocator > m_pxAllocator;
		std::unique_ptr< physx::PxDefaultErrorCallback > m_pxErrorCallback;

		physx::PxFoundation* m_pxFoundation;
		physx::PxPhysics* m_physX;

		physx::PxDefaultCpuDispatcher* m_pxDispatcher;

		std::unique_ptr< PhysicsMaterial > m_defaultMaterial;

		physx::PxPvd* m_pvd;
		physx::PxPvdTransport* m_transport;
	};
}

