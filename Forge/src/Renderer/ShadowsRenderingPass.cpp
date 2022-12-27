#include "Fpch.h"
#include "ShadowsRenderingPass.h"

const renderer::ShaderDefine renderer::ShadowsRenderingPass::c_shadowPassDefine{ "__SHADOW_PASS__" };

void renderer::ShadowsRenderingPass::OnDraw( const renderer::ICamera& camera, const renderer::IRawRenderablesPack& rawRenderables, const LightingData* lightingData )
{
	std::vector< renderer::IRenderTargetView* > views{ nullptr };
	GetRenderer().SetRenderTargets( views, &GetDepthStencilView() );

	GetRenderer().Draw( rawRenderables, &c_shadowPassDefine );
}

forge::ArraySpan< const renderer::ShaderDefine > renderer::ShadowsRenderingPass::GetRequiredShaderDefines()
{
	thread_local ShaderDefine shaderDefines[] = { c_shadowPassDefine };
	return shaderDefines;
}
