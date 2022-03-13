#include "Fpch.h"
#include "DefferedRenderingPass.h"
#include "../D3D11Renderer/D3D11RenderTargetView.h"
#include "FullScreenRenderingPass.h"
#include "ICamera.h"
#include "LightData.h"
#include "IBlendState.h"

struct CBDefferedRendering
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
	const renderer::ShaderDefine c_defferedDefine{ "__DEFFERED__" };
	const renderer::ShaderDefine c_pointLightDefine{ "__POINT_LIGHT__" };
	const renderer::ShaderDefine c_spotLightDefine{ "__SPOT_LIGHT__" };
	const renderer::ShaderDefine c_directionalLightDefine{ "__DIRECTIONAL_LIGHT__" };
}

forge::ArraySpan< const renderer::ShaderDefine > renderer::DefferedRenderingPass::GetRequiredShaderDefines()
{
	thread_local ShaderDefine shaderDefines[] = { c_defferedDefine };
	return shaderDefines;
}

renderer::DefferedRenderingPass::DefferedRenderingPass( IRenderer& renderer, std::function< const forge::ICamera&( ) > activeCameraGetter )
	: IMeshesRenderingPass( renderer )
	, m_activeCameraGetter( activeCameraGetter )
{
	m_lightingPass = std::make_unique<FullScreenRenderingPass>( GetRenderer(), "DefferedLighting.fx", "DefferedLighting.fx", forge::ArraySpan< renderer::ShaderDefine >( {} ) );

	m_cbDefferedRendering = GetRenderer().CreateStaticConstantBuffer< CBDefferedRendering >();
	m_cbPointLight = GetRenderer().CreateStaticConstantBuffer< CBPointLight >();
	m_cbSpotLight = GetRenderer().CreateStaticConstantBuffer< CBSpotLight >();
	m_cbDirectionalLight = GetRenderer().CreateStaticConstantBuffer< CBDirectionalLight >();

	m_blendState = GetRenderer().CreateBlendState( { renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE },
		{ renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE } );
}

void renderer::DefferedRenderingPass::Draw( const renderer::IRawRenderablesPack& rawRenderables, const LightingData* lightingData )
{
	std::vector< renderer::IRenderTargetView* > views{ GetTargetTexture()->GetRenderTargetView(), m_diffuseTexture->GetRenderTargetView(), m_normalsTexture->GetRenderTargetView() };
	GetRenderer().SetRenderTargets( views, GetDepthStencilBuffer() );


	StaticConstantBuffer< CBDefferedRendering >* cbRendering = static_cast<StaticConstantBuffer< CBDefferedRendering >*>( m_cbDefferedRendering.get() );
	if( lightingData )
	{
		cbRendering->GetData().AmbientLighting = lightingData->m_ambientLight;
	}

	cbRendering->GetData().InvVP = m_activeCameraGetter().GetProjectionMatrix().AffineInverted() * m_activeCameraGetter().GetInvViewMatrix();

	cbRendering->UpdateBuffer();
	cbRendering->SetVS( renderer::VSConstantBufferType::RenderingPass );
	cbRendering->SetPS( renderer::PSConstantBufferType::RenderingPass );

	GetRenderer().Draw( rawRenderables, &c_defferedDefine );

	if( lightingData )
	{
		std::vector< renderer::IShaderResourceView* > srvs =
		{
			m_diffuseTexture->GetShaderResourceView(),
			GetDepthStencilBuffer()->GetTexture()->GetShaderResourceView(),
			m_normalsTexture->GetShaderResourceView()
		};

		m_blendState->Set();

		{
			m_lightingPass->SetShaderDefines( { c_pointLightDefine } );
			StaticConstantBuffer< CBPointLight >* cbLighting = static_cast<StaticConstantBuffer< CBPointLight >*>( m_cbPointLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for( const auto& light : lightingData->m_pointLights )
			{
				cbLighting->GetData().LightingData = light;
				cbLighting->UpdateBuffer();

				m_lightingPass->Draw( srvs );
			}
		}

		{
			m_lightingPass->SetShaderDefines( { c_spotLightDefine } );
			StaticConstantBuffer< CBSpotLight >* cbLighting = static_cast<StaticConstantBuffer < CBSpotLight >*>( m_cbSpotLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for( const auto& light : lightingData->m_spotLights )
			{
				cbLighting->GetData().LightingData = light;
				cbLighting->UpdateBuffer();

				m_lightingPass->Draw( srvs );
			}
		}

		{
			m_lightingPass->SetShaderDefines( { c_directionalLightDefine } );
			StaticConstantBuffer< CBDirectionalLight >* cbLighting = static_cast<StaticConstantBuffer < CBDirectionalLight >*>( m_cbDirectionalLight.get() );
			cbLighting->SetVS( renderer::VSConstantBufferType::Light );
			cbLighting->SetPS( renderer::PSConstantBufferType::Light );

			for( const auto& light : lightingData->m_directionalLights )
			{
				cbLighting->GetData().LightingData = light;
				cbLighting->UpdateBuffer();

				m_lightingPass->Draw( srvs );
			}
		}

		m_blendState->Clear();
	}
}

void renderer::DefferedRenderingPass::ClearTargetTexture()
{
	IMeshesRenderingPass::ClearTargetTexture();
	m_lightingPass->ClearTargetTexture();
	m_diffuseTexture->GetRenderTargetView()->Clear();
	m_normalsTexture->GetRenderTargetView()->Clear();
}

void renderer::DefferedRenderingPass::SetTargetTexture( ITexture& targetTexture )
{
	renderer::IMeshesRenderingPass::SetTargetTexture( targetTexture );
	m_lightingPass->SetTargetTexture( targetTexture );

	ITexture::Flags flags = ITexture::Flags::BIND_RENDER_TARGET | ITexture::Flags::BIND_SHADER_RESOURCE;

	m_normalsTexture = GetRenderer().CreateTexture( static_cast< Uint32 >( targetTexture.GetTextureSize().X ), static_cast<Uint32>( targetTexture.GetTextureSize().Y ), flags, ITexture::Format::R8G8B8A8_UNORM, ITexture::Format::R8G8B8A8_UNORM );
	m_diffuseTexture = GetRenderer().CreateTexture( static_cast< Uint32 >( targetTexture.GetTextureSize().X ), static_cast< Uint32 >( targetTexture.GetTextureSize().Y ), flags, ITexture::Format::R8G8B8A8_UNORM, ITexture::Format::R8G8B8A8_UNORM );
}

void renderer::DefferedRenderingPass::OnTargetTextureResized( const Vector2& size )
{
	m_normalsTexture->Resize( size );
	m_diffuseTexture->Resize( size );
}