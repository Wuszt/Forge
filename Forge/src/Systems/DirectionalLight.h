#pragma once
#include "IComponent.h"
#include "../Renderer/LightData.h"

namespace forge
{
	struct DirectionalLightComponentData
	{
		DECLARE_STRUCT( DirectionalLightComponentData );

		Vector3 Direction;
		Float padding0;
		Vector3 Color = { 1.0f, 1.0f, 1.0f };
		Float padding1;
		Matrix VP;
	};

	class DirectionalLightComponent : public DataComponent< DirectionalLightComponentData >
	{
		DECLARE_TYPE( DirectionalLightComponent, forge, IComponent );
	public:
		using DataComponent::DataComponent;
	};
}
