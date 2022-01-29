#include "Fpch.h"
#include "DefferedRenderingPass.h"
#include "../D3D11Renderer/D3D11RenderTargetView.h"
#include "FullScreenRenderingPass.h"

renderer::DefferedRenderingPass::DefferedRenderingPass( IRenderer& renderer )
	: IMeshesRenderingPass( renderer )
{
	Vector2 resolution = renderer.GetResolution();
	ITexture::Flags flags = ITexture::Flags::BIND_RENDER_TARGET | ITexture::Flags::BIND_SHADER_RESOURCE;

	m_normalsTexture = GetRenderer().CreateTexture( static_cast<Uint32>( resolution.X ), static_cast<Uint32>( resolution.Y ), flags, ITexture::Format::R8G8B8A8_UNORM, ITexture::Format::R8G8B8A8_UNORM );
	m_diffuseTexture = GetRenderer().CreateTexture( static_cast<Uint32>( resolution.X ), static_cast<Uint32>( resolution.Y ), flags, ITexture::Format::R8G8B8A8_UNORM, ITexture::Format::R8G8B8A8_UNORM );

	m_lightingPass = std::make_unique<FullScreenRenderingPass>( GetRenderer(), "DefferedLighting.fx" );
}

void renderer::DefferedRenderingPass::Draw( const renderer::IRawRenderablesPack& rawRenderables )
{
	std::vector< renderer::IRenderTargetView* > views{ m_diffuseTexture->GetRenderTargetView(), m_normalsTexture->GetRenderTargetView() };
	GetRenderer().ClearShaderResourceViews();
	GetRenderer().SetRenderTargets( views, GetDepthStencilBuffer() );

	GetRenderer().Draw( rawRenderables );

	std::vector< renderer::IShaderResourceView* > srvs = { m_diffuseTexture->GetShaderResourceView(),
		GetRenderer().GetDepthStencilBuffer()->GetTexture()->GetShaderResourceView(),
		m_normalsTexture->GetShaderResourceView() };

	m_lightingPass->Draw( srvs );
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
