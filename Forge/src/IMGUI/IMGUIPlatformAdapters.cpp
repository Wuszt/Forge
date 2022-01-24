#include "Fpch.h"
#include "IMGUIPlatformAdapters.h"

#ifdef FORGE_PLATFORM_WINDOWS
#include "backends/imgui_impl_win32.h"
#include "../Core/WindowsWindow.h"
#include <Windows.h>
#endif

#ifdef FORGE_PLATFORM_WINDOWS

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

namespace windows
{
	IMGUIWindowsAdapter::IMGUIWindowsAdapter( forge::IWindow& window )
	{
		FORGE_ASSERT( dynamic_cast<const windows::WindowsWindow*>( &window ) );

		windows::WindowsWindow& windowsWindow = static_cast<windows::WindowsWindow&>( window );
		ImGui_ImplWin32_Init( windowsWindow.GetHWND() );
		m_windowEventCallbackToken = windowsWindow.RegisterWindowRawEventListener( []( HWND hwnd, Uint32 msg, Uint64 wParam, Uint64 lParam )
		{
			ImGui_ImplWin32_WndProcHandler( hwnd, msg, wParam, lParam );
		} );

		m_windowsWindow = &windowsWindow;
	}

	void IMGUIWindowsAdapter::OnNewFrame()
	{
		ImGui_ImplWin32_NewFrame();
	}

	IMGUIWindowsAdapter::~IMGUIWindowsAdapter()
	{
		ImGui_ImplWin32_Shutdown();
	}
}
#endif