#include "Fpch.h"
#include "WindowsWindow.h"
#include "WindowsInput.h"
#include "Windows.h"
#include "shobjidl_core.h"
#include "stdlib.h"

namespace
{
	LRESULT CALLBACK WindowCallbackHandler( HWND hwnd,
		Uint32 msg,
		WPARAM wParam,
		LPARAM lParam )
	{
		LONG_PTR ptr = GetWindowLongPtr( hwnd, GWLP_USERDATA );

		Bool eventHandled = false;

		if ( ptr != NULL )
		{
			windows::WindowsWindow* windowPtr = reinterpret_cast< windows::WindowsWindow* >( ptr );
			eventHandled = windowPtr->OnWindowEvent( msg, wParam, lParam );
		}

		LRESULT result = NULL;

		if ( !eventHandled )
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

	WindowsWindow::InitializationState InternalInitialize( HINSTANCE hInstance, Uint32 width, Uint32 height, const Char* windowName, HWND& outHWND )
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
		wc.hIcon = ( HICON )LoadImage( NULL, TEXT("../../forge.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED );
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 2 );
		wc.lpszMenuName = NULL;
		wc.lpszClassName = c_windowClassName;
		wc.hIconSm = wc.hIcon;

		if ( !RegisterClassEx( &wc ) )
		{
			return WindowsWindow::InitializationState::Error_Registering_Class;
		}

		RECT wr = { 0, 0, static_cast< Int32 >( width ), static_cast< Int32 >( height ) };
		AdjustWindowRect( &wr, WS_OVERLAPPEDWINDOW, false );

		std::string windowNameStr( windowName );
		outHWND = CreateWindowEx(
			NULL,
			c_windowClassName,
			std::wstring( windowNameStr.begin(), windowNameStr.end() ).c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			wr.right - wr.left, wr.bottom - wr.top,
			NULL,
			NULL,
			hInstance,
			NULL
		);

		if ( !outHWND )
		{
			return WindowsWindow::InitializationState::Error_Creating_Window;
		}

		ShowWindow( outHWND, SW_SHOWDEFAULT );
		UpdateWindow( outHWND );

		return WindowsWindow::InitializationState::Initialized;
	}

	WindowsWindow::WindowsWindow( Uint32 width, Uint32 height, const Char* windowName )
		: m_width( width )
		, m_height( height )
	{
		auto hInstance = GetModuleHandle( NULL );

		Initialize( hInstance, windowName );
		FORGE_ASSERT( IsInitialized() );

		SetWindowLongPtr( m_hwnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( this ) );

		m_input = std::make_unique< WindowsInput >( hInstance, *this );
	}

	WindowsWindow::~WindowsWindow() = default;

	void WindowsWindow::Initialize( HINSTANCE hInstance, const Char* windowName )
	{
		m_initializationState = InternalInitialize( hInstance, m_width, m_height, windowName, m_hwnd );
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

		m_positionX = rect.left + static_cast< Int32 >( xDiff / 2 );
		m_positionY = rect.top + static_cast< Int32 >( yDiff - xDiff / 2 );
	}

	void WindowsWindow::Update()
	{
		m_input->Update();

		while ( true )
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

			if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
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
		switch ( msg )
		{
		case WM_CLOSE:
			DispatchEvent( forge::IWindow::OnCloseRequestedEvent( *this ) );
			return true;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			return false;

		case WM_SIZE:
			if ( wParam != SIZE_MINIMIZED )
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

	forge::Path WindowsWindow::CreateFileDialog( FileDialogType type, forge::ArraySpan< std::string > extensions, const forge::Path& defaultPath ) const
	{
		forge::Path result;
		if ( SUCCEEDED( CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE ) ) )
		{
			IID fileDialogIID;
			CLSID fileDialogCLSID;
			switch ( type )
			{
			case forge::IWindow::FileDialogType::Open:
				fileDialogIID = IID_IFileOpenDialog;
				fileDialogCLSID = CLSID_FileOpenDialog;
				break;

			case forge::IWindow::FileDialogType::Save:
				fileDialogIID = IID_IFileSaveDialog;
				fileDialogCLSID = CLSID_FileSaveDialog;
				break;

			default:
				FORGE_FATAL();
			}

			IFileOpenDialog* fileDialog = nullptr;
			if ( SUCCEEDED( CoCreateInstance( fileDialogCLSID, nullptr, CLSCTX_ALL, fileDialogIID, reinterpret_cast< void** >( &fileDialog ) ) ) )
			{
				ON_SCOPE_EXIT( fileDialog->Release(); );

				IShellItem* currentFolder = NULL;
				if ( !defaultPath.IsEmpty() )
				{
					if ( !SUCCEEDED( SHCreateItemFromParsingName( std::wstring( defaultPath.AsString().begin(), defaultPath.AsString().end() ).c_str(), NULL, IID_PPV_ARGS( &currentFolder ) ) ) )
					{
						return {};
					}

					ON_SCOPE_EXIT( currentFolder->Release(); );
					fileDialog->SetFolder( currentFolder );
				}

				if ( !extensions.IsEmpty() )
				{
					std::wstring combinedExtentionsAsWide = std::wstring( TEXT( "*." ) ) + std::wstring( extensions[ 0 ].begin(), extensions[ 0 ].end() );
					for ( const auto& extension : extensions.Mid( 1u ) )
					{
						combinedExtentionsAsWide += std::wstring( TEXT( ";*." ) ) + std::wstring( extension.begin(), extension.end() );
					}

					COMDLG_FILTERSPEC filterSpec = { TEXT( "" ), combinedExtentionsAsWide.c_str() };
					if ( !SUCCEEDED( fileDialog->SetFileTypes( 1, &filterSpec ) ) )
					{
						return {};
					}
				}

				if ( SUCCEEDED( fileDialog->Show( nullptr ) ) )
				{
					IShellItem* item = nullptr;
					if ( SUCCEEDED( fileDialog->GetResult( &item ) ) )
					{
						ON_SCOPE_EXIT( item->Release(); );
						LPWSTR filePath;
						if ( SUCCEEDED( item->GetDisplayName( SIGDN_FILESYSPATH, &filePath ) ) )
						{
							std::string str;
							str.resize( std::wcslen( filePath ) );
							wcstombs( str.data(), filePath, str.size() );
							if ( type == forge::IWindow::FileDialogType::Save && extensions.GetSize() == 1 )
							{
								if ( str.find( '.' ) == std::string::npos )
								{
									str += "." + extensions[ 0 ];
								}
							}

							result = forge::Path( std::move( str ) );
						}
					}
				}
			}
		}

		return result;
	}

	forge::IInput& WindowsWindow::GetInput() const
	{
		FORGE_ASSERT( m_input );
		return *m_input;
	}
}
