#pragma once
#include "../Renderer/LightData.h"

namespace forge
{
	struct DirectionalLightFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( DirectionalLightFragment, ecs::Fragment );

		Vector3 Direction = Vector3::EY();
		Float padding0;
		Vector3 Color = { 1.0f, 1.0f, 1.0f };
		Float padding1;
		Matrix VP;
	};

	class DirectionalLightComponent : public DataComponent< DirectionalLightFragment >
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( DirectionalLightComponent, forge::IDataComponent );
	public:
		using DataComponent::DataComponent;

		ecs::MutableFragmentView< DirectionalLightFragment > GetData()
		{
			return GetMutableData();
		}
	};
}
