#pragma once

namespace renderer
{
	class IPixelShader
	{
	public:
		virtual ~IPixelShader() {}

		virtual void Set() const = 0;
	};
}
