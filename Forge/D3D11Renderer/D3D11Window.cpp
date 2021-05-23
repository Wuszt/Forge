#include "Fpch.h"
#include "D3D11Window.h"

namespace
{
	LRESULT CALLBACK WindowCallbackHandler( HWND hwnd,
		Uint32 msg,
		WPARAM wParam,
		LPARAM lParam )
	{
		LONG_PTR ptr = GetWindowLongPtr( hwnd, GWLP_USERDATA );


		D3D11Window* windowPtr = reinterpret_cast<D3D11Window*>( ptr );
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

const LPCTSTR c_windowClassName = "Window";

D3D11Window::InitializationState InternalInitialize( HINSTANCE hInstance, Uint32 width, Uint32 height, HWND& outHWND )
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
		return D3D11Window::InitializationState::Error_Registering_Class;
	}

	outHWND = CreateWindowEx(
		NULL,
		c_windowClassName,
		"Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if( !outHWND )
	{
		//MessageBox( NULL, "Error creating window",
		//	"Error", MB_OK | MB_ICONERROR );
		return D3D11Window::InitializationState::Error_Creating_Window;
	}

	ShowWindow( outHWND, SW_SHOWDEFAULT );
	UpdateWindow( outHWND );

	return D3D11Window::InitializationState::Initialized;
}

D3D11Window::D3D11Window( HINSTANCE hInstance, Uint32 width, Uint32 height )
{
	Initialize( hInstance, width, height );
	FORGE_ASSERT( IsInitialized() );

	SetWindowLongPtr( m_hwnd, GWLP_USERDATA, HandleToLong( this ) );
}

void D3D11Window::Initialize( HINSTANCE hInstance, Uint32 width, Uint32 height )
{
	m_initializationState = InternalInitialize( hInstance, width, height, m_hwnd );
}

Bool D3D11Window::OnWindowEvent( Uint32 msg, Uint64 wParam, Uint64 lParam )
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
