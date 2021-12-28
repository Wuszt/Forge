#pragma once

namespace renderer
{
	enum class SamplerStateFilterType
	{
		MIN_MAG_MIP_LINEAR
	};

	class ISamplerState
	{
	public:
		virtual ~ISamplerState() = default;
	};
}

