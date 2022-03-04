#pragma once
#include "IShader.h"

namespace renderer
{
	class IVertexShader : public IShader
	{
	public:
		virtual void Set() const = 0;
	};
}

