#pragma once

namespace physx
{
	class PxRigidActor;
	class PxRigidDynamic;
	class PxRigidStatic;
}

namespace physics
{
	class PhysxProxy;
	class PhysicsShape;

	class PhysicsActor
	{
	public:
		virtual ~PhysicsActor() = default;

		virtual void Initialize( PhysxProxy& proxy, Transform transform = Transform() ) = 0;

		void SetTransform( const Transform& transform );
		Transform GetTransform() const;

		void AddShape( const physics::PhysicsShape& shape );

		virtual const physx::PxRigidActor& GetActor() const = 0;
		virtual physx::PxRigidActor& GetActor() = 0;
	};

	class PhysicsDynamicActor : public PhysicsActor
	{
	public:
		PhysicsDynamicActor() = default;
		PhysicsDynamicActor( PhysicsDynamicActor&& other );
		~PhysicsDynamicActor();

		virtual void Initialize( PhysxProxy& proxy, Transform transform = Transform() ) override;

		void UpdateDensity( Float density );

		virtual const physx::PxRigidActor& GetActor() const override;
		virtual physx::PxRigidActor& GetActor() override;

	private:
		physx::PxRigidDynamic* m_actor;
	};

	class PhysicsStaticActor : public PhysicsActor
	{
	public:
		PhysicsStaticActor() = default;
		PhysicsStaticActor( PhysicsStaticActor&& other );
		~PhysicsStaticActor();

		virtual void Initialize( PhysxProxy& proxy, Transform transform = Transform() ) override;

		virtual const physx::PxRigidActor& GetActor() const override;
		virtual physx::PxRigidActor& GetActor() override;

	private:
		physx::PxRigidStatic* m_actor;
	};
}

