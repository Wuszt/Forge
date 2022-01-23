#include "Fpch.h"
#include "IRenderingPass.h"

renderer::IRenderingPass::IRenderingPass( IRenderer& renderer )
	: m_renderer( renderer )
{}

void renderer::IRenderingPass::ClearRenderTargetView()
{
	m_renderTargetView->Clear();
}

void renderer::IMeshesRenderingPass::ClearRenderTargetView()
{
	IRenderingPass::ClearRenderTargetView();

	if( m_depthStencilBuffer )
	{
		m_depthStencilBuffer->Clear();
	}
}
