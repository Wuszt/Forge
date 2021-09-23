#pragma once
#include "TransformComponent.h"

namespace systems
{
	class TransformSystem : public ECSSystem< forge::TransformComponentData >
	{
		using ECSSystem::ECSSystem;
	};
}

