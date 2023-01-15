#pragma once
#include "../Renderer/LightData.h"

namespace forge
{
	struct PointLightFragment : public ecs::Fragment
	{
		DECLARE_STRUCT( PointLightFragment, ecs::Fragment );
		REGISTER_ECS_FRAGMENT();

		Vector3 m_color = { 1.0f, 1.0f, 1.0f };
		Float m_power = 400.0f;
	};

	class PointLightComponent : public DataComponent< PointLightFragment >
	{
		DECLARE_POLYMORPHIC_CLASS( PointLightComponent, forge::IComponent );
	public:
		using DataComponent::DataComponent;

		PointLightFragment& GetData()
		{
			return GetData_Internal();
		}
	};
}

