#pragma once

namespace forge
{
	struct RenderingComponentData
	{
		Vector4 m_color;
	};

	class RenderingComponent : public DataComponent< RenderingComponentData >
	{
	public:
		using DataComponent::DataComponent;
	};
}

