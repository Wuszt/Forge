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
	const renderer::ShaderDefine c_pointLightDefine{ "__POINT_LIGHT__" };
	const renderer::ShaderDefine c_spotLightDefine{ "__SPOT_LIGHT__" };
	const renderer::ShaderDefine c_directionalLightDefine{ "__DIRECTIONAL_LIGHT__" };
	const renderer::ShaderDefine c_perspectiveCameraDefine{ "__PERSPECTIVE_CAMERA__" };
}

forge::ArraySpan< const renderer::ShaderDefine > renderer::DeferredRenderingPass::GetRequiredShaderDefines()
{
	thread_local ShaderDefine shaderDefines[] = { c_deferredDefine };
	return shaderDefines;
}

renderer::DeferredRenderingPass::DeferredRenderingPass( IRenderer& renderer )
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

void renderer::DeferredRenderingPass::Draw( const renderer::ICamera& camera, const renderer::IRawRenderablesPack& rawRenderables, const LightingData* lightingData )
{
	AdjustViewportSize();
	UpdateCameraConstantBuffer( camera );

	std::vector< renderer::IRenderTargetView* > views{ GetTargetTexture()->GetRenderTargetView(), m_diffuseTexture->GetRenderTargetView(), m_normalsTexture->GetRenderTargetView() };
	GetRenderer().SetRenderTargets( views, &GetDepthStencilView() );

	StaticConstantBuffer< CBDeferredRendering >* cbRendering = static_cast<StaticConstantBuffer< CBDeferredRendering >*>( m_cbDeferredRendering.get() );
	if( lightingData )
	{
		cbRendering->GetData().AmbientLighting = lightingData->m_ambientLight;
	}

	cbRendering->GetData().InvVP = camera.GetInvProjectionMatrix() * camera.GetInvViewMatrix();

	cbRendering->UpdateBuffer();
	cbRendering->SetVS( renderer::VSConstantBufferType::RenderingPass );
	cbRendering->SetPS( renderer::PSConstantBufferType::RenderingPass );

	GetRenderer().Draw( rawRenderables, &c_deferredDefine );

	if( lightingData )
	{
		renderer::IShaderResourceView* srvs[] =
		{
			m_diffuseTexture->GetShaderResourceView(),
			GetDepthStencilBuffer()->GetTexture()->GetShaderResourceView(),
			m_normalsTexture->GetShaderResourceView(),
			nullptr
		};

		renderer::IShaderResourceView*& shadowMapSRV = srvs[3];

		m_blendState->Set();

		{
			m_lightingPass->SetShaderDefines( camera.HasNonLinearDepth() ? forge::ArraySpan< renderer::ShaderDefine >{ c_pointLightDefine, c_perspectiveCameraDefine } : forge::ArraySpan< renderer::ShaderDefine >{ c_pointLightDefine } );
			StaticConstantBuffer< CBPointLight >* cbLighting = static_cast<StaticConstantBuffer< CBPointLight >*>( m_cbPointLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for( const auto& light : lightingData->m_pointLights )
			{
				cbLighting->GetData().LightingData = light.m_shaderData;
				cbLighting->UpdateBuffer();

				Uint32 srvsAmount = 3u;
				if( light.m_shadowMap )
				{
					shadowMapSRV = light.m_shadowMap->GetTexture()->GetShaderResourceView();
					++srvsAmount;
				}

				m_lightingPass->Draw( { srvs, srvsAmount } );
			}
		}

		{
			m_lightingPass->SetShaderDefines( camera.HasNonLinearDepth() ? forge::ArraySpan< renderer::ShaderDefine >{ c_spotLightDefine, c_perspectiveCameraDefine } : forge::ArraySpan< renderer::ShaderDefine >{ c_spotLightDefine } );
			StaticConstantBuffer< CBSpotLight >* cbLighting = static_cast<StaticConstantBuffer < CBSpotLight >*>( m_cbSpotLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for( const auto& light : lightingData->m_spotLights )
			{
				Uint32 srvsAmount = 3u;
				if( light.m_shadowMap )
				{
					shadowMapSRV = light.m_shadowMap->GetTexture()->GetShaderResourceView();
					++srvsAmount;
				}

				cbRendering->SetVS( renderer::VSConstantBufferType::RenderingPass );
				cbRendering->SetPS( renderer::PSConstantBufferType::RenderingPass );

				UpdateCameraConstantBuffer( camera );

				cbLighting->GetData().LightingData = light.m_shaderData;
				cbLighting->UpdateBuffer();

				m_lightingPass->Draw( { srvs, srvsAmount } );
			}
		}

		{
			m_lightingPass->SetShaderDefines( camera.HasNonLinearDepth() ? forge::ArraySpan< renderer::ShaderDefine >{ c_directionalLightDefine, c_perspectiveCameraDefine } : forge::ArraySpan< renderer::ShaderDefine >{ c_directionalLightDefine } );
			StaticConstantBuffer< CBDirectionalLight >* cbLighting = static_cast<StaticConstantBuffer < CBDirectionalLight >*>( m_cbDirectionalLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for( const auto& light : lightingData->m_directionalLights )
			{
				Uint32 srvsAmount = 3u;
				if( light.m_shadowMap )
				{
					shadowMapSRV = light.m_shadowMap->GetTexture()->GetShaderResourceView();
					++srvsAmount;
				}

				cbLighting->GetData().LightingData = light.m_shaderData;
				cbLighting->UpdateBuffer();

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

	m_normalsTexture = GetRenderer().CreateTexture( static_cast< Uint32 >( targetTexture.GetTextureSize().X ), static_cast<Uint32>( targetTexture.GetTextureSize().Y ), flags, ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, ITexture::Format::R8G8B8A8_UNORM );
	m_diffuseTexture = GetRenderer().CreateTexture( static_cast< Uint32 >( targetTexture.GetTextureSize().X ), static_cast< Uint32 >( targetTexture.GetTextureSize().Y ), flags, ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, ITexture::Format::R8G8B8A8_UNORM );
}

void renderer::DeferredRenderingPass::OnTargetTextureResized( const Vector2& size )
{
	m_normalsTexture->Resize( size );
	m_diffuseTexture->Resize( size );
}