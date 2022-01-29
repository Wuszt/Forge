#pragma once

namespace renderer
{
	class ITexture;

	class ISwapchain
	{
	public:
		virtual ~ISwapchain() {}

		virtual void Present() = 0;

		virtual void Resize( Uint32 width, Uint32 height ) = 0;

		virtual ITexture& GetBackBuffer() const = 0;
	};
}
