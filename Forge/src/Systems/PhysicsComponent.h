#pragma once
#include "../Physics/PhysicsActor.h"

namespace physics
{
	class PhysicsShape;
}

namespace systems
{
	class PhysicsSystem;
}

namespace forge
{
	struct PhysicsStaticFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( PhysicsStaticFragment, ecs::Fragment );
		physics::PhysicsStaticActor m_actor;
	};

	struct PhysicsDynamicFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( PhysicsDynamicFragment, ecs::Fragment );
		physics::PhysicsDynamicActor m_actor;
	};

	class IPhysicsComponent : public IComponent
	{
		DECLARE_ABSTRACT_CLASS( IPhysicsComponent, forge::IComponent );

		friend class systems::PhysicsSystem;

	public:
		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;
		virtual void OnDetached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;

		virtual void AddShape( physics::PhysicsShape&& shape );

		virtual physics::PhysicsActor& GetActor() = 0;
	};

	class PhysicsStaticComponent : public DataComponent< PhysicsStaticFragment, IPhysicsComponent >
	{
		DECLARE_POLYMORPHIC_CLASS( PhysicsStaticComponent, forge::IPhysicsComponent );

	public:
		virtual physics::PhysicsStaticActor& GetActor() override;
	};

	class PhysicsDynamicComponent : public DataComponent< PhysicsDynamicFragment, IPhysicsComponent >
	{
		DECLARE_POLYMORPHIC_CLASS( PhysicsDynamicComponent, forge::IPhysicsComponent );

	public:
		virtual physics::PhysicsDynamicActor& GetActor() override;

		void UpdateDensity( Float density );
		virtual void AddShape( physics::PhysicsShape&& shape ) override;

	private:
		Float m_density = 1.0f;
	};
}

