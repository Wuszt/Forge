#pragma once

namespace physx
{
	class PxMaterial;
}

namespace physics
{
	class PhysxProxy;

	class PhysicsMaterial
	{
	public:
		PhysicsMaterial( PhysxProxy& proxy, Float staticFriction, Float dynamicFriction, Float restitution );
		~PhysicsMaterial();

		const physx::PxMaterial& GetPhysxMaterial() const
		{
			return *m_material;
		}

	private:
		physx::PxMaterial* m_material = nullptr;
	};
}