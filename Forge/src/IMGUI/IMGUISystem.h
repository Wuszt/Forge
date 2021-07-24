#pragma once

#ifdef FORGE_PLATFORM_WINDOWS
#define FORGE_IMGUI_ENABLED
#endif

class IRenderer;
class IWindow;
class IIMGUIPlatformAdapter;
class IIMGUIRenderAPIAdapter;

class IMGUISystem
{
public:
	IMGUISystem( IWindow& window, IRenderer& renderer );
	~IMGUISystem();

	void OnNewFrame();
	void Render();

	std::unique_ptr< IIMGUIPlatformAdapter > m_platformAdapter;
	std::unique_ptr< IIMGUIRenderAPIAdapter > m_renderAPIAdapter;
};