#pragma once

namespace renderer
{
	class IDepthStencilBuffer
	{
	public:
		virtual ~IDepthStencilBuffer() = default;
		virtual void Clear() = 0;
	};
}
