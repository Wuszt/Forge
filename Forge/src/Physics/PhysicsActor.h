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
		RTTI_DECLARE_ABSTRACT_CLASS( PhysicsActor );

	public:
		virtual ~PhysicsActor() = default;

		virtual void Initialize( PhysxProxy& proxy, Transform transform = Transform() ) = 0;

		void SetTransform( const Transform& transform );
		Transform GetTransform() const;

		void AddShape( const physics::PhysicsShape& shape );

		virtual void ChangeScale( const Vector3& prevScale, const Vector3& newScale );

		virtual const physx::PxRigidActor& GetActor() const = 0;
		virtual physx::PxRigidActor& GetActor() = 0;
	};

	class PhysicsDynamicActor : public PhysicsActor
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PhysicsDynamicActor, PhysicsActor );

	public:
		enum class ForceMode
		{
			Force,
			Impulse,
			VelocityChange,
			Acceleration
		};

		PhysicsDynamicActor() = default;
		PhysicsDynamicActor( PhysicsDynamicActor&& other );
		~PhysicsDynamicActor();

		virtual void Initialize( PhysxProxy& proxy, Transform transform = Transform() ) override;

		void UpdateDensity( Float density );

		void AddForce( const Vector3& force, ForceMode forceMode );
		void AddTorque( const Vector3& torque, ForceMode forceMode );
		void ClearForce( ForceMode forceMode );
		void ClearTorque( ForceMode forceMode );

		void EnableGravity( bool enable );
		void EnableSimulation( bool enable );

		Vector3 GetVelocity() const;
		void SetVelocity( const Vector3& velocity );

		Vector3 GetAngularVelocity() const;
		void SetAngularVelocity( const Vector3& velocity );

		virtual void ChangeScale( const Vector3& prevScale, const Vector3& newScale ) override;

		virtual const physx::PxRigidActor& GetActor() const override;
		virtual physx::PxRigidActor& GetActor() override;

		const physx::PxRigidDynamic& GetDynamicActor() const
		{
			return *m_actor;
		}

		physx::PxRigidDynamic& GetDynamicActor()
		{
			return *m_actor;
		}

	private:
		physx::PxRigidDynamic* m_actor;
	};

	class PhysicsStaticActor : public PhysicsActor
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PhysicsStaticActor, PhysicsActor );

	public:
		PhysicsStaticActor() = default;
		PhysicsStaticActor( PhysicsStaticActor&& other );
		~PhysicsStaticActor();

		virtual void Initialize( PhysxProxy& proxy, Transform transform = Transform() ) override;

		virtual const physx::PxRigidActor& GetActor() const override;
		virtual physx::PxRigidActor& GetActor() override;

		const physx::PxRigidStatic& GetStaticActor() const
		{
			return *m_actor;
		}

		physx::PxRigidStatic& GetStaticActor()
		{
			return *m_actor;
		}

	private:
		physx::PxRigidStatic* m_actor;
	};
}

