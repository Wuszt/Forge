#pragma once
#include "IComponent.h"
#include "../Renderer/LightData.h"

namespace forge
{
	struct SpotLightComponentData
	{
		Vector3 m_color = { 1.0f, 1.0f, 1.0f };
		Float m_innerAngle = FORGE_PI / 12.0f;
		Float m_outerAngle = FORGE_PI / 6.0f;
		Float m_power = 400.0f;
	};

	class SpotLightComponent : public DataComponent< SpotLightComponentData >
	{
	public:
		using DataComponent::DataComponent;
	};
}