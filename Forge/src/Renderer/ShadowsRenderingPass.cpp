#include "Fpch.h"
#include "ShadowsRenderingPass.h"
#include "../ECS/Query.h"

const renderer::ShaderDefine renderer::ShadowsRenderingPass::c_shadowPassDefine{ "__SHADOW_PASS__" };

void renderer::ShadowsRenderingPass::OnDraw( const renderer::ICamera& camera, ecs::ECSManager& ecsManager, const ecs::Query& query, renderer::RenderingPass renderingPass, const LightingData* lightingData )
{
	std::vector< renderer::IRenderTargetView* > views{ nullptr };
	GetRenderer().SetRenderTargets( views, &GetDepthStencilView() );

	query.VisitArchetypes( ecsManager, [ & ]( ecs::Archetype& archetype )
		{
			GetRenderer().Draw( archetype, renderingPass, &c_shadowPassDefine );
		} );
}

forge::ArraySpan< const renderer::ShaderDefine > renderer::ShadowsRenderingPass::GetRequiredShaderDefines()
{
	thread_local ShaderDefine shaderDefines[] = { c_shadowPassDefine };
	return shaderDefines;
}
