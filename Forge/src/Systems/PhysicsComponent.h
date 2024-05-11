#pragma once
#include "../Physics/PhysicsActor.h"

namespace physics
{
	class PhysicsShape;
	enum class PhysicsGroupFlags : Uint32;
}

namespace systems
{
	class PhysicsSystem;
}

namespace forge
{
	struct PhysicsStaticFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( PhysicsStaticFragment, ecs::Fragment );
		physics::PhysicsStaticActor m_actor;
	};

	struct PhysicsDynamicFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( PhysicsDynamicFragment, ecs::Fragment );
		physics::PhysicsDynamicActor m_actor;
	};

	class PhysicsComponent : public IDataComponent
	{
		RTTI_DECLARE_ABSTRACT_CLASS( PhysicsComponent, forge::IDataComponent );

		friend class systems::PhysicsSystem;

	public:
		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;
		virtual void OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;

		virtual void AddShape( physics::PhysicsShape&& shape );

		void SetGroup( physics::PhysicsGroupFlags group );

		virtual physics::PhysicsActor& GetActor() = 0;
	};

	class PhysicsStaticComponent : public DataComponent< PhysicsStaticFragment, PhysicsComponent >
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PhysicsStaticComponent, forge::PhysicsComponent );

	public:
		virtual physics::PhysicsStaticActor& GetActor() override;
	};

	class PhysicsDynamicComponent : public DataComponent< PhysicsDynamicFragment, PhysicsComponent >
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PhysicsDynamicComponent, forge::PhysicsComponent );

	public:
		virtual physics::PhysicsDynamicActor& GetActor() override;

		void UpdateDensity( Float density );
		virtual void AddShape( physics::PhysicsShape&& shape ) override;

	private:
		Float m_density = 1.0f;
	};
}

