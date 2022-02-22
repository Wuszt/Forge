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

struct CBDefferedLighting
{
	renderer::LightData LightData;
};

renderer::DefferedRenderingPass::DefferedRenderingPass( IRenderer& renderer, std::function< const forge::ICamera&( ) > activeCameraGetter )
	: IMeshesRenderingPass( renderer )
	, m_activeCameraGetter( activeCameraGetter )
{
	Vector2 resolution = renderer.GetResolution();
	ITexture::Flags flags = ITexture::Flags::BIND_RENDER_TARGET | ITexture::Flags::BIND_SHADER_RESOURCE;

	m_normalsTexture = GetRenderer().CreateTexture( static_cast<Uint32>( resolution.X ), static_cast<Uint32>( resolution.Y ), flags, ITexture::Format::R8G8B8A8_UNORM, ITexture::Format::R8G8B8A8_UNORM );
	m_diffuseTexture = GetRenderer().CreateTexture( static_cast<Uint32>( resolution.X ), static_cast<Uint32>( resolution.Y ), flags, ITexture::Format::R8G8B8A8_UNORM, ITexture::Format::R8G8B8A8_UNORM );

	m_lightingPass = std::make_unique<FullScreenRenderingPass>( GetRenderer(), "DefferedLighting.fx", "DefferedLighting.fx" );

	m_cbDefferedRendering = GetRenderer().CreateStaticConstantBuffer< CBDefferedRendering >();
	m_CBdefferedLighting = GetRenderer().CreateStaticConstantBuffer< CBDefferedLighting >();

	m_blendState = GetRenderer().CreateBlendState( { renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE },
		{ renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ONE } );
}

void renderer::DefferedRenderingPass::Draw( const renderer::IRawRenderablesPack& rawRenderables, const LightingData& lightingData )
{
	std::vector< renderer::IRenderTargetView* > views{ GetTargetTexture()->GetRenderTargetView(), m_diffuseTexture->GetRenderTargetView(), m_normalsTexture->GetRenderTargetView() };
	GetRenderer().ClearShaderResourceViews();
	GetRenderer().SetRenderTargets( views, GetDepthStencilBuffer() );

	{
		StaticConstantBuffer< CBDefferedRendering >* cbRendering = static_cast<StaticConstantBuffer< CBDefferedRendering >*>( m_cbDefferedRendering.get() );
		cbRendering->GetData().InvVP = m_activeCameraGetter().GetProjectionMatrix().AffineInverted() * m_activeCameraGetter().GetInvViewMatrix();
		cbRendering->GetData().AmbientLighting = lightingData.m_ambientLight;
		cbRendering->UpdateBuffer();
		cbRendering->SetVS( renderer::VSConstantBufferType::RenderingPass );
		cbRendering->SetPS( renderer::PSConstantBufferType::RenderingPass );
	}

	GetRenderer().Draw( rawRenderables );

	std::vector< renderer::IShaderResourceView* > srvs =
	{
		m_diffuseTexture->GetShaderResourceView(),
		GetRenderer().GetDepthStencilBuffer()->GetTexture()->GetShaderResourceView(),
		m_normalsTexture->GetShaderResourceView()
	};

	m_blendState->Set();

	StaticConstantBuffer< CBDefferedLighting >* cbLighting = static_cast<StaticConstantBuffer< CBDefferedLighting >*>( m_CBdefferedLighting.get() );
	cbLighting->SetVS( renderer::VSConstantBufferType::Light );
	cbLighting->SetPS( renderer::PSConstantBufferType::Light );

	for( const auto& light : lightingData.m_worldLights )
	{
		cbLighting->GetData().LightData = light;
		cbLighting->UpdateBuffer();

		m_lightingPass->Draw( srvs );
	}

	m_blendState->Clear();
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
}

void renderer::DefferedRenderingPass::OnTargetTextureResized( const Vector2& size )
{
	m_normalsTexture->Resize( size );
	m_diffuseTexture->Resize( size );
}
