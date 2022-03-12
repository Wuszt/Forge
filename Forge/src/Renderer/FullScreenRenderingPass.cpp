#include "Fpch.h"
#include "FullScreenRenderingPass.h"

renderer::FullScreenRenderingPass::FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines )
	: FullScreenRenderingPass( renderer, "VS_Fullscreen.fx", fullscreenEffectPath, shaderDefines )
{}

renderer::FullScreenRenderingPass::FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenVSPath, const std::string& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines )
	: IRenderingPass( renderer )
	, m_vertexShaderName( fullscreenVSPath )
	, m_pixelShaderName( fullscreenEffectPath )
	, m_shaderDefines( shaderDefines.begin(), shaderDefines.end() )
{}

void renderer::FullScreenRenderingPass::Draw( std::vector< IShaderResourceView* > input )
{
	GetRenderer().GetShadersManager()->GetVertexShader( m_vertexShaderName, m_shaderDefines )->GetMainShader()->Set();
	GetRenderer().GetShadersManager()->GetPixelShader( m_pixelShaderName, m_shaderDefines )->GetMainShader()->Set();
	GetRenderer().SetRenderTargets( { GetTargetTexture()->GetRenderTargetView() }, nullptr );
	GetRenderer().SetShaderResourceViews( input );
	GetRenderer().DrawRawVertices( 4 );
	GetRenderer().ClearShaderResourceViews();
}
