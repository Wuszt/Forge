#pragma once
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
		DECLARE_CLASS( PointLightComponent, IComponent );
	public:
		using DataComponent::DataComponent;
	};
}

