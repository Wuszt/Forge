#pragma once

#ifdef FORGE_PLATFORM_WINDOWS
#define FORGE_IMGUI_ENABLED
#endif

namespace renderer
{
	class IIMGUIRenderAPIAdapter;
	class Renderer;
}

namespace forge
{
	class IIMGUIPlatformAdapter;
	class IWindow;

	class IMGUIInstance
	{
	public:
		IMGUIInstance( IWindow& window, renderer::Renderer& renderer );
		~IMGUIInstance();

		void OnNewFrame();
		void Render();

	private:
		std::unique_ptr< IIMGUIPlatformAdapter > m_platformAdapter;
		std::unique_ptr< renderer::IIMGUIRenderAPIAdapter > m_renderAPIAdapter;
	};
}