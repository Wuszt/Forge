#pragma once
#include "IInput.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"

struct IDirectInputDevice8;

struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;

struct HWND__;
typedef HWND__* HWND;

struct tagMSG;

namespace windows
{
	class WindowsWindow;

	class WindowsInput : public forge::IInput
	{
	public:
		WindowsInput( HINSTANCE hInstance, const WindowsWindow& window );
		~WindowsInput();

		void Update();

		void OnEvent( const tagMSG& msg );
		virtual Bool GetKey( forge::IInput::Key key ) const override { return m_keysHeld[ static_cast< Uint8 >( key ) ]; }
		virtual Bool GetKeyDown( forge::IInput::Key key ) const override { return m_keysPressed[ static_cast< Uint8 >( key ) ]; }
		virtual Bool GetKeyUp( forge::IInput::Key key ) const override { return m_keysReleased[ static_cast< Uint8 >( key ) ]; }
		virtual KeyState GetKeyState( Key key ) const override;

		virtual Bool GetMouseButton( MouseButton button ) const override { return GetKey( forge::IInput::ConvertMouseButtonToKey( button ) ); }
		virtual Bool GetMouseButtonDown( MouseButton button ) const override { return GetKeyDown( forge::IInput::ConvertMouseButtonToKey( button ) ); }
		virtual Bool GetMouseButtonUp( MouseButton button ) const override { return GetKeyUp( forge::IInput::ConvertMouseButtonToKey( button ) ); }
		virtual KeyState GetMouseButtonState( MouseButton button ) const override { return GetKeyState( forge::IInput::ConvertMouseButtonToKey( button ) ); }
		virtual Coords2D GetMouseCurrentPos() const override { return { m_mouseCurrentPos.X, m_mouseCurrentPos.Y }; };
		virtual Coords2D GetMouseDeltaPos() const override { return { m_mouseDeltaPos.X, m_mouseDeltaPos.Y }; }
		virtual Float GetMouseScrollDelta() const override { return m_scrollDelta; }

		virtual void LockCursor( Bool lock ) override
		{
			m_lockCursor = lock;
		}

		virtual Bool IsCursorLocked() const override
		{
			return m_lockCursor;
		}

		virtual [[nodiscard]] forge::CallbackToken RegisterOnInputEvent( forge::Callback< InputEvent >::TFunc callback ) override
		{
			return m_onInputEvent.AddListener( std::move( callback ) );
		}

	private:
		void OnKeyEvent( forge::IInput::KeyEvent event );
		void OnMouseWheelUpdate( Int32 delta );

		Bool m_lockCursor = false;

		constexpr static Uint8 c_keysAmount = std::numeric_limits< std::underlying_type_t< IInput::Key > >::max();
		std::bitset< c_keysAmount > m_keysPressed;
		std::bitset< c_keysAmount > m_keysHeld;
		std::bitset< c_keysAmount > m_keysReleased;

		Vector2 m_mouseCurrentPos;
		Vector2 m_mouseDeltaPos;
		
		Float m_scrollDelta = 0.0f;

		forge::Callback< InputEvent > m_onInputEvent;

		const WindowsWindow& m_window;
	};
}

