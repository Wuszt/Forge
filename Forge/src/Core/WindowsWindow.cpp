#include "Fpch.h"
#include "WindowsWindow.h"
#include <Windows.h>
#include "WindowsInput.h"

namespace
{
	LRESULT CALLBACK WindowCallbackHandler( HWND hwnd,
		Uint32 msg,
		WPARAM wParam,
		LPARAM lParam )
	{
		LONG_PTR ptr = GetWindowLongPtr( hwnd, GWLP_USERDATA );


		WindowsWindow* windowPtr = reinterpret_cast< WindowsWindow* >( ptr );
		if( windowPtr->OnWindowEvent( msg, wParam, lParam ) )
		{
			return 0;
		}

		return DefWindowProc( hwnd,
			msg,
			wParam,
			lParam );
	}
}

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
		//MessageBox( NULL, "Error registering class",
		//	"Error", MB_OK | MB_ICONERROR );
		return WindowsWindow::InitializationState::Error_Registering_Class;
	}

	RECT wr = { 0, 0, static_cast< Int32 >( width ), static_cast< Int32 >( height ) };
	AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, false );

	outHWND = CreateWindowEx(
		NULL,
		c_windowClassName,
		L"Window",
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
		//MessageBox( NULL, "Error creating window",
		//	"Error", MB_OK | MB_ICONERROR );
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

	SetWindowLongPtr( m_hwnd, GWLP_USERDATA, HandleToLong( this ) );


	m_input = std::make_unique< WindowsInput >( hInstance, *this );
}

WindowsWindow::~WindowsWindow() = default;

void WindowsWindow::Initialize( HINSTANCE hInstance )
{
	m_initializationState = InternalInitialize( hInstance, m_width, m_height, m_hwnd );
}

void WindowsWindow::Update()
{
	m_input->OnBeforeUpdate();

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
			//if( msg.message == WM_QUIT )
			//	break;

			m_input->OnEvent( msg );

			TranslateMessage( &msg );
			DispatchMessage( &msg );

			for( const auto& callback : m_onWindowEventCallbacks )
			{
				callback.second( msg.hwnd, msg.message, msg.wParam, msg.lParam );
			}
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
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE ) {
			DestroyWindow( GetHWND() );
		}

		return true;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return true;
	}

	return false;
}

Uint32 WindowsWindow::RegisterWindowEventCallback( const WindowEventCallback& callback )
{
	m_onWindowEventCallbacks.emplace( ++m_lastAvailableCallbackID, callback );
}

void WindowsWindow::UnregisterWindowEventCallback( Uint32 id )
{
	m_onWindowEventCallbacks.erase( id );
}

IInput* WindowsWindow::GetInput() const
{
	return m_input.get();
}
