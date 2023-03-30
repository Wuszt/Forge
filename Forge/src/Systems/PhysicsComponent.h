#pragma once

namespace forge
{
	struct PhysicsFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( PhysicsFragment, ecs::Fragment );
	};

	class PhysicsComponent : forge::DataComponent< PhysicsFragment >
	{
		DECLARE_POLYMORPHIC_CLASS( PhysicsComponent, forge::IComponent );
	};
}

