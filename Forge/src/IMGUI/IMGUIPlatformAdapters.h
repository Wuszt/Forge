#pragma once

namespace forge
{
	class IIMGUIPlatformAdapter
	{
	public:
		virtual ~IIMGUIPlatformAdapter() {}

		virtual void OnNewFrame() = 0;
	};
}

#ifdef FORGE_PLATFORM_WINDOWS

namespace forge
{
	class IWindow;
}

namespace windows
{
	class WindowsWindow;

	class IMGUIWindowsAdapter : public forge::IIMGUIPlatformAdapter
	{
	public:
		IMGUIWindowsAdapter( forge::IWindow& window );
		virtual void OnNewFrame();
		virtual ~IMGUIWindowsAdapter();

	private:
		windows::WindowsWindow* m_windowsWindow = nullptr;
		forge::CallbackToken m_windowEventCallbackToken;

	};
}
#endif

