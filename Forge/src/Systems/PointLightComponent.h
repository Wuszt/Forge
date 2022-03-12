#pragma once
#include "IComponent.h"
#include "../Renderer/LightData.h"

namespace forge
{
	struct PointLightComponentData
	{
		Vector3 m_color = { 1.0f, 1.0f, 1.0f };
		Float m_power = 400.0f;
	};

	class PointLightComponent : public DataComponent< PointLightComponentData >
	{
	public:
		using DataComponent::DataComponent;
	};
}

