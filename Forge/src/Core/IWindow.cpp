#include "fpch.h"
#include "IWindow.h"
#include "WindowsWindow.h"

namespace forge
{
	std::unique_ptr< IWindow > IWindow::CreateNewWindow( Uint32 width, Uint32 height, const Char* name )
	{
#ifdef FORGE_PLATFORM_WINDOWS
		return std::make_unique< windows::WindowsWindow >( width, height, name );
#endif

		FORGE_FATAL( "Creating windows for this platform is not implemented" );
	}

	IWindow::IEvent::IEvent( IWindow& window )
		: m_window( window )
	{}

	IWindow::IEvent::~IEvent() = default;
}