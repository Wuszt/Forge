#include "Fpch.h"
#include "ForwardRenderingPass.h"
#include "LightData.h"

void renderer::ForwardRenderingPass::Draw( const renderer::IRawRenderablesPack& rawRenderables, const LightingData& lightingData )
{
	std::vector< renderer::IRenderTargetView* > views{ GetTargetTexture()->GetRenderTargetView() };
	GetRenderer().SetRenderTargets( views, GetDepthStencilBuffer() );

	GetRenderer().Draw( rawRenderables );
}
