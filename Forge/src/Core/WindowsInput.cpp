#include "Fpch.h"
#include "../Math/PublicDefaults.h"
#include <dinput.h>
#include "WindowsInput.h"
#include "WindowsWindow.h"
#include <iostream>
#include <winuser.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

using namespace forge;

namespace windows
{
	WindowsInput::WindowsInput( HINSTANCE hInstance, const WindowsWindow& window )
		: m_window( window )
	{}

	WindowsInput::~WindowsInput() = default;

	void WindowsInput::Update()
	{
		m_keysPressed.reset();
		m_keysReleased.reset();
		m_scrollDelta = 0.0f;

		POINT tmp;
		GetCursorPos( &tmp );
		ScreenToClient( m_window.GetHWND(), &tmp );

		Vector2 prevPos = m_mouseCurrentPos;

		m_mouseCurrentPos = Vector2( static_cast< Float >( tmp.x ), static_cast< Float >( tmp.y ) );
		m_mouseCurrentPos.X = Math::Clamp( 0.0f, static_cast< Float >( m_window.GetWidth() ), m_mouseCurrentPos.X ) - static_cast< Float >( m_window.GetWidth() / 2u );
		m_mouseCurrentPos.Y = -( Math::Clamp( 0.0f, static_cast< Float >( m_window.GetHeight() ), m_mouseCurrentPos.Y ) - static_cast< Float >( m_window.GetHeight() / 2u ) );

		m_mouseDeltaPos = Vector2( m_mouseCurrentPos - prevPos );

		if ( m_lockCursor )
		{
			m_mouseCurrentPos = Vector2::ZEROS();

			const Uint32 x = m_window.GetPosX() + m_window.GetWidth() / 2u;
			const Uint32 y = m_window.GetPosY() + m_window.GetHeight() / 2u;

			SetCursorPos( x, y );
			while ( ShowCursor( false ) >= 0 );
		}
		else
		{
			while ( ShowCursor( true ) < 0 );
		}
	}

	void WindowsInput::OnEvent( const tagMSG& msg )
	{
		switch ( msg.message )
		{
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			OnKeyEvent( { static_cast< IInput::Key >( msg.wParam ), IInput::KeyEvent::Type::Press } );
			break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
			OnKeyEvent( { static_cast< IInput::Key >( msg.wParam ), IInput::KeyEvent::Type::Release } );
			break;

		case WM_LBUTTONDOWN:
			OnKeyEvent( { IInput::Key::LeftMouseBtn, IInput::KeyEvent::Type::Press } );
			break;

		case WM_RBUTTONDOWN:
			OnKeyEvent( { IInput::Key::RightMouseBtn, IInput::KeyEvent::Type::Press } );
			break;

		case WM_MBUTTONDOWN:
			OnKeyEvent( { IInput::Key::MidMouseBtn, IInput::KeyEvent::Type::Press } );
			break;

		case WM_LBUTTONUP:
			OnKeyEvent( { IInput::Key::LeftMouseBtn, IInput::KeyEvent::Type::Release } );
			break;

		case WM_RBUTTONUP:
			OnKeyEvent( { IInput::Key::RightMouseBtn, IInput::KeyEvent::Type::Release } );
			break;

		case WM_MBUTTONUP:
			OnKeyEvent( { IInput::Key::MidMouseBtn, IInput::KeyEvent::Type::Release } );
			break;

		case WM_MOUSEWHEEL:
			OnMouseWheelUpdate( GET_WHEEL_DELTA_WPARAM( msg.wParam ) );
			break;
		}
	}

	void WindowsInput::OnKeyEvent( IInput::KeyEvent event )
	{
		const bool isPress = event.m_type == KeyEvent::Type::Press;

		m_keysPressed[ static_cast< Uint8 >( event.m_key ) ] = !m_keysHeld[ static_cast< Uint8 >( event.m_key ) ] && isPress;
		m_keysReleased[ static_cast< Uint8 >( event.m_key ) ] = m_keysHeld[ static_cast< Uint8 >( event.m_key ) ] && !isPress;

		m_keysHeld[ static_cast< Uint8 >( event.m_key ) ] = isPress;
	}

	IInput::KeyState WindowsInput::GetKeyState( Key key ) const
	{
		if ( GetKeyDown( key ) )
		{
			return IInput::KeyState::Pressed;
		}
		else if ( GetKeyUp( key ) )
		{
			return IInput::KeyState::Released;
		}
		else if ( GetKey( key ) )
		{
			return IInput::KeyState::Held;
		}

		return IInput::KeyState::None;
	}
}