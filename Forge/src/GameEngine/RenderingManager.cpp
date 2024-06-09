#include "Fpch.h"
#include "RenderingManager.h"
#include "../Core/IWindow.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ISwapchain.h"
#include "../Renderer/IRenderTargetView.h"

renderer::RenderingManager::RenderingManager( const forge::DepotsContainer& depotsContainer, forge::AssetsManager& assetsManager, forge::UpdateManager& updateManager )
{
	const Uint32 width = 1200;
	const Uint32 height = 600;
	m_window = forge::IWindow::CreateNewWindow( width, height );
	m_renderer = renderer::Renderer::CreateRenderer( depotsContainer, assetsManager, *m_window, renderer::RendererType::D3D11 );

	m_windowUpdateToken = updateManager.RegisterUpdateFunction( forge::UpdateManager::BucketType::PostUpdate, [ this ]()
	{ 
		GetWindow().Update();
	} );

	m_prerenderingToken = updateManager.RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, [ this ]()
	{
		GetRenderer().GetSwapchain()->GetBackBuffer().GetRenderTargetView()->Clear();
		GetRenderer().OnBeforeDraw();
		GetRenderer().SetViewportSize( Vector2( static_cast< Float >( GetWindow().GetWidth() ), static_cast< Float >( GetWindow().GetHeight() ) ) );
	} );

	m_presentToken = updateManager.RegisterUpdateFunction( forge::UpdateManager::BucketType::Present, [ this ]() 
	{ 
		GetRenderer().GetSwapchain()->Present(); 
	} );
}
