#pragma once
#include "../Renderer/LightData.h"

namespace forge
{
	struct SpotLightFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( SpotLightFragment, ecs::Fragment )

		LinearColor m_color = { 1.0f, 1.0f, 1.0f };
		Float m_innerAngle = FORGE_PI / 12.0f;
		Float m_outerAngle = FORGE_PI / 6.0f;
		Float m_power = 5.0f;
	};

	class SpotLightComponent : public DataComponent< SpotLightFragment >
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( SpotLightComponent, forge::IDataComponent );
	public:
		using DataComponent::DataComponent;

		ecs::MutableFragmentView< SpotLightFragment > GetData()
		{
			return GetMutableData();
		}
	};
}
