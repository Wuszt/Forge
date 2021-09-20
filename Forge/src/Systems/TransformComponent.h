#pragma once
#include "TransformSystem.h"

namespace forge
{
	class TransformComponent : public DataComponent< TransformComponentData >
	{
	public:
		using DataComponent::DataComponent;
	};
}

