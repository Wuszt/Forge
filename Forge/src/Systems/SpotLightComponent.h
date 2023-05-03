#pragma once
#include "../Renderer/LightData.h"

namespace forge
{
	struct SpotLightFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( SpotLightFragment, ecs::Fragment )

		Vector3 m_color = { 1.0f, 1.0f, 1.0f };
		Float m_innerAngle = FORGE_PI / 12.0f;
		Float m_outerAngle = FORGE_PI / 6.0f;
		Float m_power = 5.0f;
	};

	class SpotLightComponent : public DataComponent< SpotLightFragment >
	{
		DECLARE_POLYMORPHIC_CLASS( SpotLightComponent, forge::IComponent );
	public:
		using DataComponent::DataComponent;

		SpotLightFragment& GetData()
		{
			return GetMutableData();
		}
	};
}
