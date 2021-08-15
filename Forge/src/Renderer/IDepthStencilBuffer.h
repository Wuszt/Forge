#pragma once

namespace renderer
{
	class IDepthStencilBuffer
	{
	public:
		virtual ~IDepthStencilBuffer() {}
		virtual void Clear() = 0;
	};
}
