#pragma once
#include "../Renderer/LightData.h"

namespace forge
{
	struct PointLightFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( PointLightFragment, ecs::Fragment );

		Vector3 m_color = { 1.0f, 1.0f, 1.0f };
		Float m_power = 5.0f;
	};

	class PointLightComponent : public DataComponent< PointLightFragment >
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PointLightComponent, forge::IDataComponent );
	public:
		using DataComponent::DataComponent;

		ecs::MutableFragmentView< PointLightFragment > GetData()
		{
			return GetMutableData();
		}
	};
}

