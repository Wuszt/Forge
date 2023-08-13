#include "Fpch.h"
#include "RenderingManager.h"
#include "../Core/IWindow.h"
#include "../Renderer/Renderer.h"

renderer::RenderingManager::RenderingManager( const forge::DepotsContainer& depotsContainer, forge::AssetsManager& assetsManager, forge::UpdateManager& updateManager )
{
	const Uint32 width = 1600;
	const Uint32 height = 900;
	m_window = forge::IWindow::CreateNewWindow( width, height );
	m_renderer = renderer::Renderer::CreateRenderer( depotsContainer, assetsManager, *m_window, renderer::RendererType::D3D11 );

	m_windowUpdateToken = updateManager.RegisterUpdateFunction(forge::UpdateManager::BucketType::PostUpdate, [this](){ m_window->Update(); });
}
