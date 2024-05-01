#include "Fpch.h"
#include "WindowsInput.h"
#include "../Math/PublicDefaults.h"
#include <dinput.h>
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

		POINT rawCursorPos;
		GetCursorPos( &rawCursorPos );
		ScreenToClient( m_window.GetHWND(), &rawCursorPos );

		Vector2 prevPos = m_mouseCurrentPos;

		m_mouseCurrentPos = Vector2( static_cast< Float >( rawCursorPos.x ), static_cast< Float >( rawCursorPos.y ) );
		m_mouseCurrentPos.X = Math::Clamp( 0.0f, static_cast< Float >( m_window.GetWidth() ), m_mouseCurrentPos.X ) - static_cast< Float >( m_window.GetWidth() / 2u );
		m_mouseCurrentPos.Y = -( Math::Clamp( 0.0f, static_cast< Float >( m_window.GetHeight() ), m_mouseCurrentPos.Y ) - static_cast< Float >( m_window.GetHeight() / 2u ) );

		m_mouseDeltaPos = Vector2( m_mouseCurrentPos - prevPos );

		if ( m_mouseDeltaPos.SquareMag() > 0 )
		{
			m_onInputEvent.Invoke( InputEvent{ { m_mouseDeltaPos.X, m_mouseDeltaPos.Y } } );
		}

		if ( m_lockCursor )
		{
			m_mouseCurrentPos = prevPos;
			rawCursorPos.x = static_cast< Int32 >( prevPos.X ) + m_window.GetWidth() / 2;
			rawCursorPos.y = static_cast< Int32 >( -prevPos.Y ) + m_window.GetHeight() / 2;

			ClientToScreen( m_window.GetHWND(), &rawCursorPos );
			SetCursorPos( rawCursorPos.x, rawCursorPos.y );
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

		m_onInputEvent.Invoke( event );
	}

	void WindowsInput::OnMouseWheelUpdate( Int32 delta )
	{
		m_scrollDelta += static_cast< Float >( delta );
		m_onInputEvent.Invoke( static_cast< Float >( delta ) );
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