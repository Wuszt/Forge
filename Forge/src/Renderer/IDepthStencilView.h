#pragma once

namespace renderer
{
	class IDepthStencilView
	{
	public:
		virtual ~IDepthStencilView() = default;
		virtual void Clear() = 0;
	};
}
