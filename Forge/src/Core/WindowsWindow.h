#pragma once
#include "IWindow.h"
#include <functional>

struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;

struct HWND__;
typedef HWND__* HWND;

class WindowsInput;

class WindowsWindow : public IWindow
{
public:

	using WindowEventCallback = std::function< void( HWND, Uint32, Uint64, Uint64 ) >;

	enum class InitializationState
	{
		NotInitialized,
		Initialized,
		Error_Creating_Window,
		Error_Registering_Class
	};

	WindowsWindow( Uint32 width, Uint32 height );
	~WindowsWindow();

	FORGE_INLINE HWND GetHWND() const
	{
		FORGE_ASSERT( IsInitialized() );

		return m_hwnd;
	}

	FORGE_INLINE Bool IsInitialized() const
	{
		return m_initializationState == InitializationState::Initialized;
	}

	FORGE_INLINE InitializationState GetInitializationState() const
	{
		return m_initializationState;
	}

	virtual void Update() override;

	Bool OnWindowEvent( Uint32 msg, Uint64 wParam, Uint64 lParam );

	FORGE_INLINE virtual Uint32 GetHeight() const override
	{
		return m_height;
	}

	FORGE_INLINE virtual Uint32 GetWidth() const override
	{
		return m_width;
	}

	Uint32 RegisterWindowEventCallback( const WindowEventCallback& callback );
	void UnregisterWindowEventCallback( Uint32 id );

	virtual IInput* GetInput() const override;

private:

	void Initialize( HINSTANCE hInstance );

	HWND m_hwnd = nullptr;
	InitializationState m_initializationState = InitializationState::NotInitialized;

	Uint32 m_width = 0u;
	Uint32 m_height = 0u;

	std::unique_ptr< WindowsInput > m_input;
	std::unordered_map< Uint32, WindowEventCallback > m_onWindowEventCallbacks;
	Uint32 m_lastAvailableCallbackID = 0;
};

