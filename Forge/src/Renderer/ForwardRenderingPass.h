#pragma once
#include "IRenderingPass.h"

namespace renderer
{
	struct LightData;

	class ForwardRenderingPass : public IMeshesRenderingPass 
	{
	public:
		using IMeshesRenderingPass::IMeshesRenderingPass;
		virtual void Draw( const renderer::IRawRenderablesPack& rawRenderables, const LightingData& lightingData ) override;
	};
}

