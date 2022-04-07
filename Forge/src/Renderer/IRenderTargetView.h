#pragma once

namespace renderer
{
	class IRenderTargetView
	{
	public:
		virtual ~IRenderTargetView() {}

		virtual void Clear( const Vector4& bgColor = { 0.0f, 0.0f, 0.0f, 0.0f } ) = 0;
	};
}
