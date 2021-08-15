#include "fpch.h"
#include "IWindow.h"
#include "WindowsWindow.h"

namespace forge
{
	std::unique_ptr< IWindow > IWindow::CreateNewWindow( Uint32 width, Uint32 height )
	{
#ifdef FORGE_PLATFORM_WINDOWS
		return std::make_unique< windows::WindowsWindow >( width, height );
#endif

		FORGE_FATAL( "Creating windows for this platform is not implemented" );
	}
}