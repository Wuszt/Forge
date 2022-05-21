#pragma once

namespace renderer
{
	enum class DepthStencilComparisonFunc
	{
		COMPARISON_NEVER,
		COMPARISON_LESS,
		COMPARISON_EQUAL,
		COMPARISON_LESS_EQUAL,
		COMPARISON_GREATER,
		COMPARISON_NOT_EQUAL,
		COMPARISON_GREATER_EQUAL,
		COMPARISON_ALWAYS
	};

	class IDepthStencilState
	{
	public:
		virtual void Set() = 0;
		virtual void Clear() = 0;
		virtual void EnableWrite( Bool enable ) = 0;

		virtual ~IDepthStencilState() = default;
	};
}

