#pragma once

struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;

struct HWND__;
typedef HWND__* HWND;

class D3D11Window
{
public:
	enum class InitializationState
	{
		NotInitialized,
		Initialized,
		Error_Creating_Window,
		Error_Registering_Class
	};

	D3D11Window( HINSTANCE hInstance, Uint32 width, Uint32 height );

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

	Bool OnWindowEvent( Uint32 msg, Uint64 wParam, Uint64 lParam );

private:

	void Initialize( HINSTANCE hInstance, Uint32 width, Uint32 height );

	HWND m_hwnd = nullptr;
	InitializationState m_initializationState = InitializationState::NotInitialized;
};

