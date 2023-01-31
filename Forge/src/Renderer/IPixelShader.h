#pragma once
#include "IShader.h"

namespace renderer
{
	class IPixelShader : public IShader
	{
	public:
		virtual void Set() const = 0;
	};
}
