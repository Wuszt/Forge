#pragma once

struct Vector3;
struct Vector2;

namespace forge
{
	class IWindow;

	class IInput
	{
	public:
		enum class Key
		{
			LeftMouseBtn = 0x01,
			RightMouseBtn = 0x02,
			CtrlBrkPrcs = 0x03,
			MidMouseBtn = 0x04,

			ThumbForward = 0x05,
			ThumbBack = 0x06,

			BackSpace = 0x08,
			Tab = 0x09,

			Clear = 0x0C,
			Enter = 0x0D,

			Shift = 0x10,
			Control = 0x11,
			Alt = 0x12,
			Pause = 0x13,
			CapsLock = 0x14,

			Kana = 0x15,
			Hangeul = 0x15,
			Hangul = 0x15,

			Junju = 0x17,
			Final = 0x18,
			Hanja = 0x19,
			Kanji = 0x19,

			Escape = 0x1B,

			Convert = 0x1C,
			NonConvert = 0x1D,
			Accept = 0x1E,
			ModeChange = 0x1F,

			Space = 0x20,
			PageUp = 0x21,
			PageDown = 0x22,
			End = 0x23,
			Home = 0x24,
			LeftArrow = 0x25,
			UpArrow = 0x26,
			RightArrow = 0x27,
			DownArrow = 0x28,
			Select = 0x29,
			Print = 0x2A,
			Execute = 0x2B,
			PrintScreen = 0x2C,
			Inser = 0x2D,
			Delete = 0x2E,
			Help = 0x2F,

			Num0 = 0x30,
			Num1 = 0x31,
			Num2 = 0x32,
			Num3 = 0x33,
			Num4 = 0x34,
			Num5 = 0x35,
			Num6 = 0x36,
			Num7 = 0x37,
			Num8 = 0x38,
			Num9 = 0x39,

			A = 0x41,
			B = 0x42,
			C = 0x43,
			D = 0x44,
			E = 0x45,
			F = 0x46,
			G = 0x47,
			H = 0x48,
			I = 0x49,
			J = 0x4A,
			K = 0x4B,
			L = 0x4C,
			M = 0x4D,
			N = 0x4E,
			O = 0x4F,
			P = 0x50,
			Q = 0x51,
			R = 0x52,
			S = 0x53,
			T = 0x54,
			U = 0x55,
			V = 0x56,
			W = 0x57,
			X = 0x58,
			Y = 0x59,
			Z = 0x5A,

			LeftWin = 0x5B,
			RightWin = 0x5C,
			Apps = 0x5D,

			Sleep = 0x5F,

			Numpad0 = 0x60,
			Numpad1 = 0x61,
			Numpad2 = 0x62,
			Numpad3 = 0x63,
			Numpad4 = 0x64,
			Numpad5 = 0x65,
			Numpad6 = 0x66,
			Numpad7 = 0x67,
			Numpad8 = 0x68,
			Numpad9 = 0x69,
			Multiply = 0x6A,
			Add = 0x6B,
			Separator = 0x6C,
			Subtract = 0x6D,
			Decimal = 0x6E,
			Divide = 0x6F,
			F1 = 0x70,
			F2 = 0x71,
			F3 = 0x72,
			F4 = 0x73,
			F5 = 0x74,
			F6 = 0x75,
			F7 = 0x76,
			F8 = 0x77,
			F9 = 0x78,
			F10 = 0x79,
			F11 = 0x7A,
			F12 = 0x7B,

			LeftShift = 0xA0,
			RightShift = 0xA1,
			LeftCtrl = 0xA2,
			RightCtrl = 0xA3,
			LeftMenu = 0xA4,
			RightMenu = 0xA5,
		};

		enum class MouseButton
		{
			LeftButton = static_cast<Uint32>( Key::LeftMouseBtn ),
			RightButton = static_cast<Uint32>( Key::RightMouseBtn ),
			MiddleButton = static_cast<Uint32>( Key::MidMouseBtn ),
		};

		virtual ~IInput() = default;

		virtual Bool GetKey( Key key ) const = 0;
		virtual Bool GetKeyDown( Key key ) const = 0;
		virtual Bool GetKeyUp( Key key ) const = 0;
		virtual const Vector3& GetMouseDeltaAxises() const = 0;
		virtual Bool GetMouseButton( MouseButton button ) const = 0;
		virtual Bool GetMouseButtonDown( MouseButton button ) const = 0;
		virtual Bool GetMouseButtonUp( MouseButton button ) const = 0;
		virtual const Vector2& GetMouseCurrentAxises() const = 0;
		virtual void LockCursor( Bool lock ) = 0;
		virtual Bool IsCursorLocked() const = 0;
	};
}