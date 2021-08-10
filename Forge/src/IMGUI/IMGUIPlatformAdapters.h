#pragma once

class WindowsWindow;

class IIMGUIPlatformAdapter
{
public:
	virtual ~IIMGUIPlatformAdapter() {}

	virtual void OnNewFrame() = 0;
};

#ifdef FORGE_PLATFORM_WINDOWS

class IWindow;

class IMGUIWindowsAdapter : public IIMGUIPlatformAdapter
{
public:
	IMGUIWindowsAdapter( IWindow& window );
	virtual void OnNewFrame();
	virtual ~IMGUIWindowsAdapter();

private:
	WindowsWindow* m_windowsWindow = nullptr;
	std::unique_ptr< CallbackToken > m_windowEventCallbackToken;

};
#endif

