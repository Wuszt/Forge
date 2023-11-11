#include "Fpch.h"
#include "ShadowsRenderingPass.h"
#include "../ECS/Query.h"
#include "../ECS/Archetype.h"

const renderer::ShaderDefine renderer::ShadowsRenderingPass::c_shadowPassDefine{ "__SHADOW_PASS__" };

void renderer::ShadowsRenderingPass::OnDraw( const renderer::ICamera& camera, const ecs::Query& query, renderer::RenderingPass renderingPass, const LightingData* lightingData )
{
	renderer::IRenderTargetView* views[] = { nullptr };
	GetRenderer().SetRenderTargets( views, &GetDepthStencilView() );

	ecs::Query queryCopy = query;
	queryCopy.AddTagRequirement< renderer::WireFrameTag >( ecs::Query::RequirementType::Excluded );
	queryCopy.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
		{
			GetRenderer().Draw( archetype, renderingPass, &c_shadowPassDefine );
		} );

	{
		GetRenderer().SetFillMode( FillMode::WireFrame );
		GetRenderer().SetCullingMode( renderer::CullingMode::None );
		queryCopy.RemoveTagRequirement< renderer::WireFrameTag >();
		queryCopy.AddTagRequirement< renderer::WireFrameTag >( ecs::Query::RequirementType::Included );
		queryCopy.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
			{
				GetRenderer().Draw( archetype, renderingPass, &c_shadowPassDefine );
			} );
		GetRenderer().SetFillMode( FillMode::Solid );
		GetRenderer().SetCullingMode( renderer::CullingMode::CullingBack );
	}
}

forge::ArraySpan< const renderer::ShaderDefine > renderer::ShadowsRenderingPass::GetRequiredShaderDefines()
{
	thread_local ShaderDefine shaderDefines[] = { c_shadowPassDefine };
	return shaderDefines;
}
