#include "Fpch.h"
#include "D3D11Window.h"

LRESULT CALLBACK WndProc( HWND hWnd,
	Uint32 msg,
	WPARAM wParam,
	LPARAM lParam );

const LPCTSTR c_windowClassName = "Window";

D3D11Window::D3D11Window( HINSTANCE hInstance, Uint32 width, Uint32 height )
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
	wc.lpfnWndProc = WndProc;
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
		MessageBox( NULL, "Error registering class",
			"Error", MB_OK | MB_ICONERROR );
		//return 1;
	}

	HWND hwnd = CreateWindowEx(
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

	if( !hwnd )
	{
		MessageBox( NULL, "Error creating window",
			"Error", MB_OK | MB_ICONERROR );
		//return 1;
	}

	ShowWindow( hwnd, SW_SHOWDEFAULT );
	UpdateWindow( hwnd );
}
