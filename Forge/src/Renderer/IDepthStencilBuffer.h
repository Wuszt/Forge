#pragma once

namespace renderer
{
	class ITexture;

	class IDepthStencilBuffer
	{
	public:
		virtual ~IDepthStencilBuffer() = default;
		virtual std::shared_ptr< renderer::ITexture > GetTexture() const = 0;
		virtual void Clear() = 0;
	};
}
