#include "Fpch.h"
#include "DeferredRenderingPass.h"
#include "../D3D11Renderer/D3D11RenderTargetView.h"
#include "FullScreenRenderingPass.h"
#include "ICamera.h"
#include "LightData.h"
#include "IBlendState.h"
#include "ForwardRenderingPass.h"
#include "CustomCamera.h"
#include "ShadowMapsGenerator.h"
#include "../ECS/Query.h"
#include "../ECS/Archetype.h"
#include "Renderer.h"

struct CBDeferredRendering
{
	Matrix InvVP;
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

namespace
{
	const renderer::ShaderDefine c_deferredDefine{ "__DEFERRED__" };
	const renderer::ShaderDefine c_perspectiveCameraDefine{ "__PERSPECTIVE_CAMERA__" };
	static const renderer::ShaderDefine c_shadowMapDefine{ "__SHADOW_MAP__" };
}

forge::ArraySpan< const renderer::ShaderDefine > renderer::DeferredRenderingPass::GetRequiredShaderDefines()
{
	thread_local ShaderDefine shaderDefines[] = { c_deferredDefine };
	return shaderDefines;
}

renderer::DeferredRenderingPass::DeferredRenderingPass( Renderer& renderer )
	: IMeshesRenderingPass( renderer )
{
	m_lightingPass = std::make_unique< FullScreenRenderingPass >( GetRenderer(), "DeferredLighting.fx", "DeferredLighting.fx", forge::ArraySpan< renderer::ShaderDefine >( {} ) );

	m_cbDeferredRendering = GetRenderer().CreateStaticConstantBuffer< CBDeferredRendering >();
	m_cbPointLight = GetRenderer().CreateStaticConstantBuffer< CBPointLight >();
	m_cbSpotLight = GetRenderer().CreateStaticConstantBuffer< CBSpotLight >();
	m_cbDirectionalLight = GetRenderer().CreateStaticConstantBuffer< CBDirectionalLight >();

	m_blendState = GetRenderer().CreateBlendState( { renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE },
		{ renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE } );
}

void renderer::DeferredRenderingPass::OnDraw( const renderer::ICamera& camera, const ecs::Query& query, renderer::RenderingPass renderingPass, const LightingData* lightingData )
{
	renderer::IRenderTargetView* views[] = { GetTargetTexture()->GetRenderTargetView(), m_diffuseTexture->GetRenderTargetView(), m_normalsTexture->GetRenderTargetView() };
	GetRenderer().SetRenderTargets( views, &GetDepthStencilView() );

	StaticConstantBuffer< CBDeferredRendering >* cbRendering = static_cast< StaticConstantBuffer< CBDeferredRendering >* >( m_cbDeferredRendering.get() );
	if ( lightingData )
	{
		cbRendering->GetData().AmbientLighting = lightingData->m_ambientLight;
	}

	cbRendering->GetData().InvVP = camera.GetInvProjectionMatrix() * camera.GetInvViewMatrix();

	cbRendering->UpdateBuffer();
	cbRendering->SetVS( renderer::VSConstantBufferType::RenderingPass );
	cbRendering->SetPS( renderer::PSConstantBufferType::RenderingPass );

	{
		ecs::Query noWireFrameDrawing = query;
		noWireFrameDrawing.AddTagRequirement< renderer::WireFrameTag >( ecs::Query::RequirementType::Excluded );
		noWireFrameDrawing.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
			{
				GetRenderer().Draw( archetype, renderingPass, &c_deferredDefine );
			} );
	}

	{
		ecs::Query wireFrameDrawing = query;
		GetRenderer().SetCullingMode( renderer::CullingMode::None );
		GetRenderer().SetFillMode( FillMode::WireFrame );
		wireFrameDrawing.AddTagRequirement< renderer::WireFrameTag >( ecs::Query::RequirementType::Included );
		wireFrameDrawing.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
			{
				GetRenderer().Draw( archetype, renderingPass, &c_deferredDefine );
			} );
		GetRenderer().SetFillMode( FillMode::Solid );
		GetRenderer().SetCullingMode( renderer::CullingMode::CullingBack );
	}

	if ( lightingData )
	{
		const renderer::IShaderResourceView* srvs[] =
		{
			m_diffuseTexture->GetShaderResourceView(),
			GetDepthStencilBuffer()->GetTexture()->GetShaderResourceView(),
			m_normalsTexture->GetShaderResourceView(),
			nullptr
		};

		const renderer::IShaderResourceView*& shadowMapSRV = srvs[ 3 ];

		m_blendState->Set();

		std::vector< renderer::ShaderDefine > shaderDefines = { renderer::ShaderDefine() };
		if ( camera.HasNonLinearDepth() )
		{
			shaderDefines.emplace_back( c_perspectiveCameraDefine );
		}

		shaderDefines.emplace_back( c_shadowMapDefine );

		auto setShaderDefinesFunc = [ & ]( Bool hasShadowMap )
		{
			m_lightingPass->SetShaderDefines( { shaderDefines, static_cast< Uint32 >( shaderDefines.size() ) - ( hasShadowMap ? 0u : 1u ) } );
		};

		ShaderDefine& lightDefine = shaderDefines[ 0 ];

		{
			lightDefine = c_pointLightDefine;
			StaticConstantBuffer< CBPointLight >* cbLighting = static_cast< StaticConstantBuffer< CBPointLight >* >( m_cbPointLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for ( const auto& light : lightingData->m_pointLights )
			{
				cbLighting->GetData().LightingData = light.m_shaderData;
				cbLighting->UpdateBuffer();

				Uint32 srvsAmount = 3u;
				if ( light.m_shadowMap )
				{
					shadowMapSRV = light.m_shadowMap->GetTexture()->GetShaderResourceView();
					++srvsAmount;
				}

				setShaderDefinesFunc( light.m_shadowMap != nullptr );
				m_lightingPass->Draw( { srvs, srvsAmount } );
			}
		}

		{
			lightDefine = c_spotLightDefine;
			StaticConstantBuffer< CBSpotLight >* cbLighting = static_cast< StaticConstantBuffer < CBSpotLight >* >( m_cbSpotLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for ( const auto& light : lightingData->m_spotLights )
			{
				Uint32 srvsAmount = 3u;
				if ( light.m_shadowMap )
				{
					shadowMapSRV = light.m_shadowMap->GetTexture()->GetShaderResourceView();
					++srvsAmount;
				}

				cbRendering->SetVS( renderer::VSConstantBufferType::RenderingPass );
				cbRendering->SetPS( renderer::PSConstantBufferType::RenderingPass );

				UpdateCameraConstantBuffer( camera );

				cbLighting->GetData().LightingData = light.m_shaderData;
				cbLighting->UpdateBuffer();

				setShaderDefinesFunc( light.m_shadowMap != nullptr );
				m_lightingPass->Draw( { srvs, srvsAmount } );
			}
		}

		{
			lightDefine = c_directionalLightDefine;
			StaticConstantBuffer< CBDirectionalLight >* cbLighting = static_cast< StaticConstantBuffer < CBDirectionalLight >* >( m_cbDirectionalLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for ( const auto& light : lightingData->m_directionalLights )
			{
				Uint32 srvsAmount = 3u;
				if ( light.m_shadowMap )
				{
					shadowMapSRV = light.m_shadowMap->GetTexture()->GetShaderResourceView();
					++srvsAmount;
				}

				cbLighting->GetData().LightingData = light.m_shaderData;
				cbLighting->UpdateBuffer();

				setShaderDefinesFunc( light.m_shadowMap != nullptr );
				m_lightingPass->Draw( { srvs, srvsAmount } );
			}
		}

		m_blendState->Clear();
	}
}

void renderer::DeferredRenderingPass::ClearTargetTexture()
{
	IMeshesRenderingPass::ClearTargetTexture();
	m_lightingPass->ClearTargetTexture();
	m_diffuseTexture->GetRenderTargetView()->Clear();
	m_normalsTexture->GetRenderTargetView()->Clear();
}

void renderer::DeferredRenderingPass::SetTargetTexture( ITexture& targetTexture )
{
	renderer::IMeshesRenderingPass::SetTargetTexture( targetTexture );
	m_lightingPass->SetTargetTexture( targetTexture );

	ITexture::Flags flags = ITexture::Flags::BIND_RENDER_TARGET | ITexture::Flags::BIND_SHADER_RESOURCE;

	m_normalsTexture = GetRenderer().CreateTexture( static_cast< Uint32 >( targetTexture.GetSize().X ), static_cast< Uint32 >( targetTexture.GetSize().Y ), flags, ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, ITexture::Format::R8G8B8A8_UNORM );
	m_diffuseTexture = GetRenderer().CreateTexture( static_cast< Uint32 >( targetTexture.GetSize().X ), static_cast< Uint32 >( targetTexture.GetSize().Y ), flags, ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, ITexture::Format::R8G8B8A8_UNORM );
}

void renderer::DeferredRenderingPass::OnTargetTextureResized( const Vector2& size )
{
	m_normalsTexture->Resize( size );
	m_diffuseTexture->Resize( size );
}