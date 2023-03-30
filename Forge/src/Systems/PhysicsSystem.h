#pragma once
#include "../GameEngine/ISystem.h"
#include "../Physics/PhysxProxy.h"

namespace systems
{
	class PhysicsSystem : public ISystem
	{
		DECLARE_POLYMORPHIC_CLASS( PhysicsSystem, systems::ISystem );

	public:
		using ISystem::ISystem;

	private:
		//physics::PhysicsProxy m_physicsProxy;
	};
}

