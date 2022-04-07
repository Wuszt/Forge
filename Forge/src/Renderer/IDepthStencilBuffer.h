#pragma once

namespace renderer
{
	class ITexture;
	class IDepthStencilView;

	class IDepthStencilBuffer
	{
	public:
		virtual ~IDepthStencilBuffer() = default;
		virtual std::shared_ptr< renderer::ITexture > GetTexture() const = 0;
		virtual IDepthStencilView& GetView( Uint32 index = 0u ) const = 0;
		virtual void Resize( Uint32 width, Uint32 height ) = 0;
	};
}
