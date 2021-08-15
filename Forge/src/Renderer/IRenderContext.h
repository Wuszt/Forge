#pragma once

namespace renderer
{
	class IRenderContext
	{
	public:
		virtual ~IRenderContext() {}

		virtual void Draw( Uint32 indexCount, Uint32 offset ) = 0;
	};
}
