#include "Fpch.h"
#include "FullScreenRenderingPass.h"
#include "Renderer.h"

renderer::FullScreenRenderingPass::FullScreenRenderingPass( Renderer& renderer, const forge::Path& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines )
	: FullScreenRenderingPass( renderer, forge::Path( "VS_Fullscreen.fx" ), fullscreenEffectPath, shaderDefines )
{}

renderer::FullScreenRenderingPass::FullScreenRenderingPass( Renderer& renderer, const forge::Path& fullscreenVSPath, const forge::Path& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines )
	: IRenderingPass( renderer )
	, m_vertexShaderPath( fullscreenVSPath )
	, m_pixelShaderPath( fullscreenEffectPath )
	, m_shaderDefines( shaderDefines.begin(), shaderDefines.end() )
{}

void renderer::FullScreenRenderingPass::Draw( forge::ArraySpan< const IShaderResourceView* > input )
{
	AdjustViewportSize();

	GetRenderer().GetShadersManager()->GetVertexShader( m_vertexShaderPath, m_shaderDefines, false )->GetMainShader()->Set();
	GetRenderer().GetShadersManager()->GetPixelShader( m_pixelShaderPath, m_shaderDefines, false )->GetMainShader()->Set();
	GetRenderer().SetRenderTargets( { GetTargetTexture()->GetRenderTargetView() }, nullptr );

	GetRenderer().SetShaderResourceViews( input );
	GetRenderer().DrawRawVertices( 4 );
	GetRenderer().ClearShaderResourceViews();
}

void renderer::FullScreenRenderingPass::AdjustViewportSize()
{
	GetRenderer().SetViewportSize( GetTargetTexture()->GetSize() );
}
