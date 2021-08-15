#pragma once
#include "IWindow.h"
#include <functional>

struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;

struct HWND__;
typedef HWND__* HWND;

namespace windows
{
	class WindowsInput;

	class WindowsWindow : public forge::IWindow
	{
	public:
		using WindowRawEventCallback = forge::Callback< HWND, Uint32, Uint64, Uint64 >;

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

		FORGE_INLINE virtual Uint32 GetPosX() const override
		{
			return m_positionX;
		}

		FORGE_INLINE virtual Uint32 GetPosY() const override
		{
			return m_positionY;
		}

		FORGE_INLINE std::unique_ptr< forge::CallbackToken > RegisterWindowRawEventListener( const WindowRawEventCallback::TFunc& callback )
		{
			return m_rawEventCallback.AddListener( callback );
		}

		virtual forge::IInput* GetInput() const override;

	private:

		void Initialize( HINSTANCE hInstance );

		void UpdatePositionAndSize();

		HWND m_hwnd = nullptr;
		InitializationState m_initializationState = InitializationState::NotInitialized;

		Uint32 m_width = 0u;
		Uint32 m_height = 0u;

		Int32 m_positionX = 0;
		Int32 m_positionY = 0;

		std::unique_ptr< WindowsInput > m_input;
		WindowRawEventCallback m_rawEventCallback;
	};
}

