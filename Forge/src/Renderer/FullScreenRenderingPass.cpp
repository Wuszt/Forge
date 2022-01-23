#include "Fpch.h"
#include "FullScreenRenderingPass.h"

renderer::FullScreenRenderingPass::FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenEffectPath )
	: IRenderingPass( renderer )
	, m_pixelShaderName( fullscreenEffectPath )
{}

void renderer::FullScreenRenderingPass::Draw( std::vector< IShaderResourceView* > input )
{
	GetRenderer().GetShadersManager()->GetVertexShader( "VS_Fullscreen.fx" )->Set();
	GetRenderer().GetShadersManager()->GetPixelShader( m_pixelShaderName )->Set();
	GetRenderer().SetRenderTargets( { GetRenderTargetView() }, nullptr );
	GetRenderer().SetShaderResourceViews( input );
	GetRenderer().DrawRawVertices( 4 );
}
