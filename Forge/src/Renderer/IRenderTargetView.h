#pragma once

namespace renderer
{
	class ITexture;
	class IRenderTargetView
	{
	public:
		virtual ~IRenderTargetView() {}

		virtual std::shared_ptr< ITexture > GetTexture() const = 0;

		virtual void Clear( const Vector4& bgColor = { 0.0f, 0.0f, 0.0f, 0.0f } ) = 0;
	};
}
