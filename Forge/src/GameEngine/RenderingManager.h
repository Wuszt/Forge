#pragma once

namespace forge
{
	class IWindow;
	class ApplicationInstance;
}

namespace renderer
{
	class Renderer;

	class RenderingManager
	{
	public:
		RenderingManager( const forge::DepotsContainer& depotsContainer, forge::AssetsManager& assetsManager, forge::UpdateManager& updateManager, forge::ApplicationInstance& appInstance );

		renderer::Renderer& GetRenderer() const
		{
			return *m_renderer;
		}

		forge::IWindow& GetWindow() const
		{
			return *m_window;
		}

	private:
		Float m_renderingResolutionScale = 1.0f;

		std::unique_ptr< renderer::Renderer > m_renderer;
		std::unique_ptr< forge::IWindow > m_window;

		forge::CallbackToken m_windowUpdateToken;
		forge::CallbackToken m_prerenderingToken;
		forge::CallbackToken m_presentToken;
	};
}

