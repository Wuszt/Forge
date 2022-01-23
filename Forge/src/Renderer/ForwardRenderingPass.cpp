#include "Fpch.h"
#include "ForwardRenderingPass.h"

void renderer::ForwardRenderingPass::Draw( const renderer::IRawRenderablesPack& rawRenderables )
{
	std::vector< renderer::IRenderTargetView* > views{ GetRenderTargetView() };
	GetRenderer().SetRenderTargets( views, GetDepthStencilBuffer() );

	GetRenderer().Draw( rawRenderables );
}
