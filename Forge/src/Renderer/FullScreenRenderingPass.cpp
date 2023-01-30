#include "Fpch.h"
#include "FullScreenRenderingPass.h"
#include "IRenderer.h"

renderer::FullScreenRenderingPass::FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines )
	: FullScreenRenderingPass( renderer, "VS_Fullscreen.fx", fullscreenEffectPath, shaderDefines )
{}

renderer::FullScreenRenderingPass::FullScreenRenderingPass( IRenderer& renderer, const std::string& fullscreenVSPath, const std::string& fullscreenEffectPath, forge::ArraySpan< renderer::ShaderDefine > shaderDefines )
	: IRenderingPass( renderer )
	, m_vertexShaderName( fullscreenVSPath )
	, m_pixelShaderName( fullscreenEffectPath )
	, m_shaderDefines( shaderDefines.begin(), shaderDefines.end() )
{}

void renderer::FullScreenRenderingPass::Draw( forge::ArraySpan< const IShaderResourceView* > input )
{
	AdjustViewportSize();

	GetRenderer().GetShadersManager()->GetVertexShader( m_vertexShaderName, m_shaderDefines, false )->GetMainShader()->Set();
	GetRenderer().GetShadersManager()->GetPixelShader( m_pixelShaderName, m_shaderDefines, false )->GetMainShader()->Set();
	GetRenderer().SetRenderTargets( { GetTargetTexture()->GetRenderTargetView() }, nullptr );

	GetRenderer().SetShaderResourceViews( input );
	GetRenderer().DrawRawVertices( 4 );
	GetRenderer().ClearShaderResourceViews();
}

void renderer::FullScreenRenderingPass::AdjustViewportSize()
{
	GetRenderer().SetViewportSize( GetTargetTexture()->GetTextureSize() );
}
