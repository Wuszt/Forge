#include "Fpch.h"
#include "IRenderingPass.h"

renderer::IRenderingPass::IRenderingPass( IRenderer& renderer )
	: m_renderer( renderer )
{}

void renderer::IRenderingPass::ClearTargetTexture()
{
	m_targetTexture->GetRenderTargetView()->Clear();
}

void renderer::IRenderingPass::SetTargetTexture( ITexture& targetTexture )
{
	m_targetTexture = &targetTexture;
	m_onTargetTextureResized = m_targetTexture->GetOnResizedCallback().AddListener( [ & ]( const Vector2& size ) { OnTargetTextureResized( size ); } );
}

void renderer::IMeshesRenderingPass::ClearTargetTexture()
{
	IRenderingPass::ClearTargetTexture();

	if( m_depthStencilBuffer )
	{
		m_depthStencilBuffer->Clear();
	}
}
