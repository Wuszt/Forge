#pragma once

namespace renderer
{
	class IVertexShader
	{
	public:
		virtual ~IVertexShader() {}

		virtual void Set() = 0;
	};
}

