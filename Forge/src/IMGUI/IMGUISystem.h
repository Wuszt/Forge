#pragma once

#ifdef FORGE_PLATFORM_WINDOWS
#define FORGE_IMGUI_ENABLED
#endif

namespace renderer
{
	class IIMGUIRenderAPIAdapter;
	class IRenderer;
}

namespace forge
{
	class IIMGUIPlatformAdapter;
	class IWindow;

	class IMGUISystem
	{
	public:
		IMGUISystem( IWindow& window, renderer::IRenderer& renderer );
		~IMGUISystem();

		void OnNewFrame();
		void Render();

		std::unique_ptr< IIMGUIPlatformAdapter > m_platformAdapter;
		std::unique_ptr< renderer::IIMGUIRenderAPIAdapter > m_renderAPIAdapter;
	};
}