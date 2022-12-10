#pragma once
#include "../Renderer/LightData.h"

namespace forge
{
	struct DirectionalLightFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( DirectionalLightFragment, ecs::Fragment );
		REGISTER_ECS_FRAGMENT();

		Vector3 Direction;
		Float padding0;
		Vector3 Color = { 1.0f, 1.0f, 1.0f };
		Float padding1;
		Matrix VP;
	};

	class DirectionalLightComponent : public DataComponent< DirectionalLightFragment >
	{
		DECLARE_POLYMORPHIC_CLASS( DirectionalLightComponent, forge::IComponent );
	public:
		using DataComponent::DataComponent;
	};
}
