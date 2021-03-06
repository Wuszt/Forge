#pragma once
#include "IComponent.h"
#include "../Renderer/LightData.h"

namespace forge
{
	struct PointLightComponentData
	{
		DECLARE_STRUCT( PointLightComponentData );

		Vector3 m_color = { 1.0f, 1.0f, 1.0f };
		Float m_power = 400.0f;
	};

	class PointLightComponent : public DataComponent< PointLightComponentData >
	{
		DECLARE_TYPE( PointLightComponent, IComponent );
	public:
		using DataComponent::DataComponent;
	};
}

