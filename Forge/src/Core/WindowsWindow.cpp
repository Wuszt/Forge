#include "Fpch.h"
#include "WindowsWindow.h"
#include "WindowsInput.h"
#include "Windows.h"

namespace
{
	LRESULT CALLBACK WindowCallbackHandler( HWND hwnd,
		Uint32 msg,
		WPARAM wParam,
		LPARAM lParam )
	{
		LONG_PTR ptr = GetWindowLongPtr( hwnd, GWLP_USERDATA );

		Bool eventHandled = false;

		if( ptr != NULL )
		{
			windows::WindowsWindow* windowPtr = reinterpret_cast< windows::WindowsWindow* >( ptr );
			eventHandled = windowPtr->OnWindowEvent( msg, wParam, lParam );
		}

		LRESULT result = NULL;

		if( !eventHandled )
		{
			result = DefWindowProc( hwnd,
				msg,
				wParam,
				lParam );
		}

		return result;
	}
}

namespace windows
{
	const LPCTSTR c_windowClassName = L"Window";

	WindowsWindow::InitializationState InternalInitialize( HINSTANCE hInstance, Uint32 width, Uint32 height, HWND& outHWND )
	{
		typedef struct _WNDCLASS {
			Uint32 cbSize;
			Uint32 style;
			WNDPROC lpfnWndProc;
			Int32 cbClsExtra;
			Int32 cbWndExtra;
			HANDLE hInstance;
			HICON hIcon;
			HCURSOR hCursor;
			HBRUSH hbrBackground;
			LPCTSTR lpszMenuName;
			LPCTSTR lpszClassName;
		} WNDCLASS;

		WNDCLASSEX wc;

		wc.cbSize = sizeof( WNDCLASSEX );
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowCallbackHandler;
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 2 );
		wc.lpszMenuName = NULL;
		wc.lpszClassName = c_windowClassName;
		wc.hIconSm = LoadIcon( NULL, IDI_APPLICATION );

		if( !RegisterClassEx( &wc ) )
		{
			return WindowsWindow::InitializationState::Error_Registering_Class;
		}

		RECT wr = { 0, 0, static_cast<Int32>( width ), static_cast<Int32>( height ) };
		AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, false );

		outHWND = CreateWindowEx(
			NULL,
			c_windowClassName,
			L"Forge",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			wr.right - wr.left, wr.bottom - wr.top,
			NULL,
			NULL,
			hInstance,
			NULL
		);

		if( !outHWND )
		{
			return WindowsWindow::InitializationState::Error_Creating_Window;
		}

		ShowWindow( outHWND, SW_SHOWDEFAULT );
		UpdateWindow( outHWND );

		return WindowsWindow::InitializationState::Initialized;
	}

	WindowsWindow::WindowsWindow( Uint32 width, Uint32 height )
		: m_width( width )
		, m_height( height )
	{
		auto hInstance = GetModuleHandle( NULL );

		Initialize( hInstance );
		FORGE_ASSERT( IsInitialized() );

		SetWindowLongPtr( m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( this ) );

		m_input = std::make_unique< WindowsInput >( hInstance, *this );
	}

	WindowsWindow::~WindowsWindow() = default;

	void WindowsWindow::Initialize( HINSTANCE hInstance )
	{
		m_initializationState = InternalInitialize( hInstance, m_width, m_height, m_hwnd );
		UpdatePositionAndSize();
	}

	void WindowsWindow::UpdatePositionAndSize()
	{
		RECT rect;
		::GetClientRect( m_hwnd, &rect );

		m_width = rect.right - rect.left;
		m_height = rect.bottom - rect.top;

		::GetWindowRect( m_hwnd, &rect );

		Int32 windowWidth = rect.right - rect.left;
		Int32 windowHeight = rect.bottom - rect.top;

		Uint32 xDiff = windowWidth - m_width;
		Uint32 yDiff = windowHeight - m_height;

		m_positionX = rect.left + static_cast<Int32>( xDiff / 2 );
		m_positionY = rect.top + static_cast<Int32>( yDiff - xDiff / 2 );
	}

	void WindowsWindow::Update()
	{
		m_input->Update();

		while( true )
		{
			MSG msg;
			ZeroMemory( &msg, sizeof( MSG ) );

			BOOL PeekMessageL(
				LPMSG lpMsg,
				HWND hWnd,
				Uint32 wMsgFilterMin,
				Uint32 wMsgFilterMax,
				Uint32 wRemoveMsg
			);

			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				m_input->OnEvent( msg );

				TranslateMessage( &msg );
				DispatchMessage( &msg );

				m_rawEventCallback.Invoke( msg.hwnd, msg.message, msg.wParam, msg.lParam );
			}
			else
			{
				break;
			}
		}
	}

	Bool WindowsWindow::OnWindowEvent( Uint32 msg, Uint64 wParam, Uint64 lParam )
	{
		switch( msg )
		{
		case WM_CLOSE:
			DispatchEvent( forge::IWindow::OnCloseRequestedEvent( *this ) );
			return true;

		case WM_DESTROY:			
			PostQuitMessage( 0 );
			return false;

		case WM_SIZE:
			if( wParam != SIZE_MINIMIZED )
			{
				UpdatePositionAndSize();
				DispatchEvent( forge::IWindow::OnResizedEvent( *this, m_width, m_height ) );
				m_rawEventCallback.Invoke( m_hwnd, msg, wParam, lParam );
				return false;
			}

		case WM_EXITSIZEMOVE:
			UpdatePositionAndSize();
			return false;
		}

		return false;
	}

	forge::IInput& WindowsWindow::GetInput() const
	{
		FORGE_ASSERT( m_input );
		return *m_input;
	}
}
