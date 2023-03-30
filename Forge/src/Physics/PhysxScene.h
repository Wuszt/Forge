#pragma once

namespace physx
{
	class PxScene;
	class PxPhysics;
	class PxDefaultCpuDispatcher;
}

namespace physics
{
	class PhysxScene
	{
	public:
		PhysxScene( physx::PxPhysics* physX, physx::PxDefaultCpuDispatcher* dispatcher );
		~PhysxScene();

	private:
		physx::PxScene* m_pxScene;
	};
}

