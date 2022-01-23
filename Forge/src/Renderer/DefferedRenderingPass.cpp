#include "Fpch.h"
#include "DefferedRenderingPass.h"
#include "../D3D11Renderer/D3D11RenderTargetView.h"
#include "FullScreenRenderingPass.h"

renderer::DefferedRenderingPass::DefferedRenderingPass( IRenderer& renderer )
	: IMeshesRenderingPass( renderer )
{
	Vector2 resolution = renderer.GetResolution();
	ITexture::Flags flags = ITexture::Flags::BIND_RENDER_TARGET | ITexture::Flags::BIND_SHADER_RESOURCE;
	{
		std::shared_ptr< ITexture > texture = GetRenderer().CreateTexture( static_cast<Uint32>( resolution.X ), static_cast<Uint32>( resolution.Y ), flags, ITexture::Format::R8G8B8A8_UNORM, ITexture::Format::R8G8B8A8_UNORM );
		m_normalsTargetView = GetRenderer().CreateRenderTargetView( texture );
	}

	{
		std::shared_ptr< ITexture > texture = GetRenderer().CreateTexture( static_cast<Uint32>( resolution.X ), static_cast<Uint32>( resolution.Y ), flags, ITexture::Format::R8G8B8A8_UNORM, ITexture::Format::R8G8B8A8_UNORM );
		m_diffuseTargetView = GetRenderer().CreateRenderTargetView( texture );
	}

	m_lightingPass = std::make_unique<FullScreenRenderingPass>( GetRenderer(), "DefferedLighting.fx" );
}

void renderer::DefferedRenderingPass::Draw( const renderer::IRawRenderablesPack& rawRenderables )
{
	std::vector< renderer::IRenderTargetView* > views{ m_diffuseTargetView.get(), m_normalsTargetView.get() };
	GetRenderer().ClearShaderResourceViews();
	GetRenderer().SetRenderTargets( views, GetDepthStencilBuffer() );

	GetRenderer().Draw( rawRenderables );

	std::vector< renderer::IShaderResourceView* > srvs = { m_diffuseTargetView->GetTexture()->GetShaderResourceView(),
		GetRenderer().GetDepthStencilBuffer()->GetTexture()->GetShaderResourceView(),
		m_normalsTargetView->GetTexture()->GetShaderResourceView() };

	m_lightingPass->Draw( srvs );
}

void renderer::DefferedRenderingPass::ClearRenderTargetView()
{
	IMeshesRenderingPass::ClearRenderTargetView();
	m_lightingPass->ClearRenderTargetView();
	m_normalsTargetView->Clear();
	m_diffuseTargetView->Clear();
}

void renderer::DefferedRenderingPass::SetRenderTargetView( IRenderTargetView* renderTargetView )
{
	renderer::IMeshesRenderingPass::SetRenderTargetView( renderTargetView );
	m_lightingPass->SetRenderTargetView( renderTargetView );
}
