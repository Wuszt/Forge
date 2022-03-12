#pragma once
#include "TransformComponent.h"

namespace systems
{
	class TransformSystem : public ECSSystem< systems::ArchetypeDataTypes< forge::TransformComponentData > >
	{
		using ECSSystem::ECSSystem;
	};
}

