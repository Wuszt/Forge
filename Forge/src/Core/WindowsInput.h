#pragma once
#include "IInput.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"

struct IDirectInputDevice8;
struct Vector2;
struct Vector3;

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

		void OnBeforeUpdate();

		void OnEvent( const tagMSG& msg );
		virtual Bool GetKey( forge::IInput::Key key ) const override;
		virtual Bool GetKeyDown( forge::IInput::Key key ) const override;
		virtual Bool GetKeyUp( forge::IInput::Key key ) const override;
		virtual KeyState GetKeyState( Key key ) const override;

		virtual const Vector3& GetMouseDeltaAxises() const override;
		virtual Bool GetMouseButton( MouseButton button ) const override;
		virtual Bool GetMouseButtonDown( MouseButton button ) const override;
		virtual Bool GetMouseButtonUp( MouseButton button ) const override;
		virtual KeyState GetMouseButtonState( MouseButton button ) const override;
		virtual const Vector2& GetMouseCurrentAxises() const override;

		virtual void LockCursor( Bool lock ) override
		{
			m_lockCursor = lock;
		}

		virtual Bool IsCursorLocked() const override
		{
			return m_lockCursor;
		}

	private:
		void OnKeyboardUpdate( forge::IInput::Key key, Bool pressed );
		void OnMouseUpdate( forge::IInput::Key key, Bool pressed );
		void OnMouseWheelUpdate( Int32 delta );

		Bool m_lockCursor = false;

		static constexpr Uint32 c_keysAmount = 256;

		std::bitset< c_keysAmount > m_keys;
		std::bitset< c_keysAmount > m_keysPressed;
		std::bitset< c_keysAmount > m_keysReleased;

		Vector2 m_mouseCurrentAxises;
		Vector3 m_mouseDeltaAxises;

		const WindowsWindow& m_window;
	};
}

