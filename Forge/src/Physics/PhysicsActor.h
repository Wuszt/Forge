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
		PhysicsActor() = default;

		virtual void Initialize( PhysxProxy& proxy, Uint32 group, Transform transform = Transform(), void* userData = nullptr ) = 0;

		void SetTransform( const Transform& transform );
		Transform GetTransform() const;

		void SetGroup( Uint32 group );
		Uint32 GetGroup() const
		{
			return m_group;
		}

		void AddShape( physics::PhysicsShape&& shape );

		virtual void ChangeScale( const Vector3& newScale );

		virtual const physx::PxRigidActor& GetActor() const = 0;
		virtual physx::PxRigidActor& GetActor() = 0;

	protected:
		const Vector3& GetCurrentScale() const
		{
			return m_currentScale;
		}

	private:
		Uint32 m_group = 0u;
		Vector3 m_currentScale = Vector3::ONES();
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

		virtual void Initialize( PhysxProxy& proxy, Uint32 group, Transform transform = Transform(), void* userData = nullptr ) override;

		void SetDensity( Float density );

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

		virtual void ChangeScale( const Vector3& newScale ) override;

		// TODO: don't return actor, implement API instead
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

		virtual void Initialize( PhysxProxy& proxy, Uint32 group, Transform transform = Transform(), void* userData = nullptr ) override;

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

