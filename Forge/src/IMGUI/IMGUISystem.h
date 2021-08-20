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
		FORGE_INLINE forge::CallbackToken AddOverlayListener( const forge::Callback<>::TFunc& func )
		{
			return m_overlayCallback.AddListener( func );
		}

	private:
		void DrawOverlay();

		std::unique_ptr< IIMGUIPlatformAdapter > m_platformAdapter;
		std::unique_ptr< renderer::IIMGUIRenderAPIAdapter > m_renderAPIAdapter;

		forge::Callback<> m_overlayCallback;
	};
}