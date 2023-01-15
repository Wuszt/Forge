#include "Fpch.h"
#include "ForwardRenderingPass.h"
#include "LightData.h"
#include "IBlendState.h"
#include "../ECS/Query.h"
#include "../ECS/Archetype.h"
#include "IRenderer.h"

struct CBForwardRendering
{
	Vector4 AmbientLighting;
};

struct CBPointLight
{
	renderer::PointLightData LightingData;
};

struct CBSpotLight
{
	renderer::SpotLightData LightingData;
};

struct CBDirectionalLight
{
	renderer::DirectionalLightData LightingData;
};

forge::ArraySpan< const renderer::ShaderDefine > renderer::ForwardRenderingPass::GetRequiredShaderDefines()
{
	thread_local ShaderDefine shaderDefines[] = { c_ambientLightDefine, c_pointLightDefine, c_spotLightDefine, c_directionalLightDefine };
	return shaderDefines;
}

renderer::ForwardRenderingPass::ForwardRenderingPass( IRenderer& renderer )
	: IMeshesRenderingPass( renderer )
{
	m_cbForwardRendering = GetRenderer().CreateStaticConstantBuffer< CBForwardRendering >();
	m_cbPointLight = GetRenderer().CreateStaticConstantBuffer< CBPointLight >();
	m_cbSpotLight = GetRenderer().CreateStaticConstantBuffer< CBSpotLight >();
	m_cbDirectionalLight = GetRenderer().CreateStaticConstantBuffer< CBDirectionalLight >();

	m_blendState = GetRenderer().CreateBlendState( { renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE },
		{ renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE } );
}

renderer::ForwardRenderingPass::~ForwardRenderingPass() = default;

void renderer::ForwardRenderingPass::OnDraw( const renderer::ICamera& camera, ecs::ECSManager& ecsManager, const ecs::Query& query, renderer::RenderingPass renderingPass, const LightingData* lightingData )
{
	std::vector< renderer::IRenderTargetView* > views{ GetTargetTexture() ? GetTargetTexture()->GetRenderTargetView() : nullptr };
	GetRenderer().SetRenderTargets( views, &GetDepthStencilView() );

	StaticConstantBuffer< CBForwardRendering >* cbRendering = static_cast< StaticConstantBuffer< CBForwardRendering >* >( m_cbForwardRendering.get() );
	const ShaderDefine* shaderDefine = nullptr;
	if ( lightingData )
	{
		cbRendering->GetData().AmbientLighting = lightingData->m_ambientLight;
		shaderDefine = &c_ambientLightDefine;
	}

	cbRendering->UpdateBuffer();
	cbRendering->SetVS( renderer::VSConstantBufferType::RenderingPass );
	cbRendering->SetPS( renderer::PSConstantBufferType::RenderingPass );

	query.VisitArchetypes( ecsManager, [ & ]( ecs::Archetype& archetype )
		{
			GetRenderer().Draw( archetype, renderingPass, shaderDefine );
		} );

	if ( lightingData )
	{
		m_blendState->Set();

		{
			StaticConstantBuffer< CBPointLight >* cbLighting = static_cast< StaticConstantBuffer< CBPointLight >* >( m_cbPointLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for ( const auto& light : lightingData->m_pointLights )
			{
				cbLighting->GetData().LightingData = light.m_shaderData;
				cbLighting->UpdateBuffer();

				IShaderResourceView* shadowMapSrv = light.m_shadowMap ? light.m_shadowMap->GetTexture()->GetShaderResourceView() : nullptr;

				forge::ArraySpan< renderer::IShaderResourceView* > srvs = { &shadowMapSrv, shadowMapSrv ? 1u : 0u };
				query.VisitArchetypes( ecsManager, [ & ]( ecs::Archetype& archetype )
					{
						GetRenderer().Draw( archetype, renderingPass, &c_pointLightDefine, srvs );
					} );
			}
		}

		{
			StaticConstantBuffer< CBSpotLight >* cbLighting = static_cast< StaticConstantBuffer < CBSpotLight >* >( m_cbSpotLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for ( const auto& light : lightingData->m_spotLights )
			{
				cbLighting->GetData().LightingData = light.m_shaderData;
				cbLighting->UpdateBuffer();

				IShaderResourceView* shadowMapSrv = light.m_shadowMap ? light.m_shadowMap->GetTexture()->GetShaderResourceView() : nullptr;

				forge::ArraySpan< renderer::IShaderResourceView* > srvs = { &shadowMapSrv, shadowMapSrv ? 1u : 0u };
				query.VisitArchetypes( ecsManager, [ & ]( ecs::Archetype& archetype )
					{
						GetRenderer().Draw( archetype, renderingPass, &c_spotLightDefine, srvs );
					} );
			}
		}

		{
			StaticConstantBuffer< CBDirectionalLight >* cbLighting = static_cast< StaticConstantBuffer < CBDirectionalLight >* >( m_cbDirectionalLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for ( const auto& light : lightingData->m_directionalLights )
			{
				cbLighting->GetData().LightingData = light.m_shaderData;
				cbLighting->UpdateBuffer();

				IShaderResourceView* shadowMapSrv = light.m_shadowMap ? light.m_shadowMap->GetTexture()->GetShaderResourceView() : nullptr;

				forge::ArraySpan< renderer::IShaderResourceView* > srvs = { &shadowMapSrv, shadowMapSrv ? 1u : 0u };
				query.VisitArchetypes( ecsManager, [ & ]( ecs::Archetype& archetype )
					{
						GetRenderer().Draw( archetype, renderingPass, &c_directionalLightDefine, srvs );
					} );
			}
		}

		m_blendState->Clear();
	}
}
