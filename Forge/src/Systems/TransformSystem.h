#pragma once
#include "TransformComponent.h"

namespace systems
{
	class TransformSystem : public ECSSystem< systems::ArchetypeDataTypes< forge::TransformComponentData > >
	{
		DECLARE_CLASS( TransformSystem, systems, IECSSystem );
		using ECSSystem::ECSSystem;
	};
}

