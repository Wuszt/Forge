#pragma once

namespace renderer
{
	class IPixelShader
	{
	public:
		~IPixelShader() {}

		virtual void Set() const = 0;
	};
}
