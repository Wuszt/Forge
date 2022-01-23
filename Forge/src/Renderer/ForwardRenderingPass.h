#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	class ForwardRenderingPass : public IMeshesRenderingPass 
	{
	public:
		using IMeshesRenderingPass::IMeshesRenderingPass;
		virtual void Draw( const renderer::IRawRenderablesPack& rawRenderables ) override;
	};
}

