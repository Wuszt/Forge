#include "fpch.h"
#include "IWindow.h"
#include "WindowsWindow.h"

std::unique_ptr< IWindow > IWindow::CreateNewWindow( Uint32 width, Uint32 height )
{
	return std::make_unique< WindowsWindow >( width, height );
}