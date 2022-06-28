#pragma once
#include "TransformComponent.h"

namespace systems
{
	class TransformSystem : public ECSSystem< systems::ArchetypeDataTypes< forge::TransformComponentData > >
	{
		DECLARE_TYPE( TransformSystem, systems, IECSSystem );
		using ECSSystem::ECSSystem;
	};
}

