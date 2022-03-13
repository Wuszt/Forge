#pragma once
#include "IComponent.h"
#include "../Renderer/LightData.h"

namespace forge
{
	using DirectionalLightComponentData = renderer::DirectionalLightData;

	class DirectionalLightComponent : public DataComponent< DirectionalLightComponentData >
	{
	public:
		using DataComponent::DataComponent;
	};
}
