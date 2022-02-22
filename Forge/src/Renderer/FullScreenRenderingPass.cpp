#include "Fpch.h"
#include "FullScreenRenderingPass.h"

renderer::FullScreenRenderingPass::FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenEffectPath )
	: FullScreenRenderingPass( renderer, "VS_Fullscreen.fx", fullscreenEffectPath )
{}

renderer::FullScreenRenderingPass::FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenVSPath, const std::string& fullscreenEffectPath )
	: IRenderingPass( renderer )
	, m_vertexShaderName( fullscreenVSPath )
	, m_pixelShaderName( fullscreenEffectPath )
{}

void renderer::FullScreenRenderingPass::Draw( std::vector< IShaderResourceView* > input )
{
	GetRenderer().GetShadersManager()->GetVertexShader( m_vertexShaderName )->Set();
	GetRenderer().GetShadersManager()->GetPixelShader( m_pixelShaderName )->Set();
	GetRenderer().SetRenderTargets( { GetTargetTexture()->GetRenderTargetView() }, nullptr );
	GetRenderer().SetShaderResourceViews( input );
	GetRenderer().DrawRawVertices( 4 );
}
