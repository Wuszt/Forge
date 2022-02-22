#pragma once
#include "IComponent.h"
#include "../Renderer/LightData.h"

namespace forge
{
	struct LightComponentData
	{
		Vector3 m_color = { 1.0f, 1.0f, 1.0f };
		Float m_power = 400.0f;
	};

	class LightComponent : public DataComponent< LightComponentData >
	{
		using DataComponent::DataComponent;
	};
}

