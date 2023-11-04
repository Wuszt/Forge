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
	{
		memset( m_keys.data(), 0, sizeof( Bool ) * 256 );
		memset( m_keysPressed.data(), 0, sizeof( Bool ) * 256 );
		memset( m_keysReleased.data(), 0, sizeof( Bool ) * 256 );
	}

	WindowsInput::~WindowsInput() = default;

	void WindowsInput::OnBeforeUpdate()
	{
		memset( m_keysPressed.data(), 0, sizeof( Bool ) * 256 );
		memset( m_keysReleased.data(), 0, sizeof( Bool ) * 256 );

		POINT tmp;
		GetCursorPos( &tmp );
		ScreenToClient( m_window.GetHWND(), &tmp );

		Vector2 prevPos = m_mouseCurrentAxises;

		m_mouseCurrentAxises = Vector2( static_cast< Float >( tmp.x ), static_cast< Float >( tmp.y ) );
		m_mouseCurrentAxises.X = Math::Clamp( 0.0f, static_cast< Float >( m_window.GetWidth() ), m_mouseCurrentAxises.X ) - static_cast< Float >( m_window.GetWidth() / 2u );
		m_mouseCurrentAxises.Y = -( Math::Clamp( 0.0f, static_cast< Float >( m_window.GetHeight() ), m_mouseCurrentAxises.Y ) - static_cast< Float >( m_window.GetHeight() / 2u ) );

		m_mouseDeltaAxises = Vector3( m_mouseCurrentAxises - prevPos, 0.0f );

		if ( m_lockCursor )
		{
			m_mouseCurrentAxises = Vector2::ZEROS();

			Uint32 x = m_window.GetPosX() + m_window.GetWidth() / 2u;
			Uint32 y = m_window.GetPosY() + m_window.GetHeight() / 2u;

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
			OnKeyboardUpdate( static_cast< IInput::Key >( msg.wParam ), true );
			break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
			OnKeyboardUpdate( static_cast< IInput::Key >( msg.wParam ), false );
			break;

		case WM_LBUTTONDOWN:
			OnMouseUpdate( IInput::Key::LeftMouseBtn, true );
			break;

		case WM_RBUTTONDOWN:
			OnMouseUpdate( IInput::Key::RightMouseBtn, true );
			break;

		case WM_MBUTTONDOWN:
			OnMouseUpdate( IInput::Key::MidMouseBtn, true );
			break;

		case WM_LBUTTONUP:
			OnMouseUpdate( IInput::Key::LeftMouseBtn, false );
			break;

		case WM_RBUTTONUP:
			OnMouseUpdate( IInput::Key::RightMouseBtn, false );
			break;

		case WM_MBUTTONUP:
			OnMouseUpdate( IInput::Key::MidMouseBtn, false );
			break;

		case WM_MOUSEWHEEL:
			OnMouseWheelUpdate( GET_WHEEL_DELTA_WPARAM( msg.wParam ) );
			break;
		}
	}

	void WindowsInput::OnKeyboardUpdate( IInput::Key key, Bool pressed )
	{
		FORGE_ASSERT( key != IInput::Key::LeftMouseBtn && key != IInput::Key::MidMouseBtn && key != IInput::Key::RightMouseBtn );

		m_keysPressed[ static_cast< Uint32 >( key ) ] = !m_keys[ static_cast< Uint32 >( key ) ] && pressed;
		m_keysReleased[ static_cast< Uint32 >( key ) ] = m_keys[ static_cast< Uint32 >( key ) ] && !pressed;

		m_keys[ static_cast< Uint32 >( key ) ] = pressed;
	}

	void WindowsInput::OnMouseUpdate( IInput::Key key, Bool pressed )
	{
		FORGE_ASSERT( key == IInput::Key::LeftMouseBtn || key == IInput::Key::MidMouseBtn || key == IInput::Key::RightMouseBtn );

		m_keysPressed[ static_cast< Uint32 >( key ) ] = !m_keys[ static_cast< Uint32 >( key ) ] && pressed;
		m_keysReleased[ static_cast< Uint32 >( key ) ] = m_keys[ static_cast< Uint32 >( key ) ] && !pressed;

		m_keys[ static_cast< Uint32 >( key ) ] = pressed;
	}

	void WindowsInput::OnMouseWheelUpdate( Int32 delta )
	{
		m_mouseDeltaAxises.Z += static_cast< Float >( delta );
	}

	Bool WindowsInput::GetKey( IInput::Key key ) const
	{
		return m_keys[ static_cast< Uint32 >( key ) ];
	}

	Bool WindowsInput::GetKeyDown( IInput::Key key ) const
	{
		return m_keysPressed[ static_cast< Uint32 >( key ) ];
	}

	Bool WindowsInput::GetKeyUp( IInput::Key key ) const
	{
		return m_keysReleased[ static_cast< Uint32 >( key ) ];
	}

	forge::IInput::KeyState WindowsInput::GetKeyState( Key key ) const
	{
		if ( GetKeyDown( key ) )
		{
			return IInput::KeyState::Clicked;
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

	const Vector3& WindowsInput::GetMouseDeltaAxises() const
	{
		return m_mouseDeltaAxises;
	}

	Bool WindowsInput::GetMouseButton( MouseButton button ) const
	{
		return m_keys[ static_cast< Uint32 >( button ) ];
	}

	Bool WindowsInput::GetMouseButtonDown( MouseButton button ) const
	{
		return m_keysPressed[ static_cast< Uint32 >( button ) ];
	}

	Bool WindowsInput::GetMouseButtonUp( MouseButton button ) const
	{
		return m_keysReleased[ static_cast< Uint32 >( button ) ];
	}

	forge::IInput::KeyState WindowsInput::GetMouseButtonState( MouseButton button ) const
	{
		if ( GetMouseButtonDown( button ) )
		{
			return IInput::KeyState::Clicked;
		}
		else if ( GetMouseButtonUp( button ) )
		{
			return IInput::KeyState::Released;
		}
		else if ( GetMouseButton( button ) )
		{
			return IInput::KeyState::Held;
		}

		return IInput::KeyState::None;
	}

	const Vector2& WindowsInput::GetMouseCurrentAxises() const
	{
		return m_mouseCurrentAxises;
	}
}