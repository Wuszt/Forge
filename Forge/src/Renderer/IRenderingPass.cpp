#include "Fpch.h"
#include "IRenderingPass.h"

renderer::IRenderingPass::IRenderingPass( IRenderer& renderer )
	: m_renderer( renderer )
{}

void renderer::IRenderingPass::ClearTargetTexture()
{
	m_targetTexture->GetRenderTargetView()->Clear();
}

void renderer::IMeshesRenderingPass::ClearTargetTexture()
{
	IRenderingPass::ClearTargetTexture();

	if( m_depthStencilBuffer )
	{
		m_depthStencilBuffer->Clear();
	}
}
