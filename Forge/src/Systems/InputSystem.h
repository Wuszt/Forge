#pragma once
#include "../GameEngine/ISystem.h"
#include "../Core/IInput.h"

namespace forge
{
	class InputHandler;
};

namespace systems
{
	class InputSystem : public systems::ISystem
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( InputSystem, systems::ISystem );

	public:
		enum class HandlerSource : Uint8
		{
			Game,
			Overlay
		};

		enum class CursorStateRequest : Uint8
		{
			None,
			Enabled,
			Disabled
		};

		struct InputHandlerDesc
		{
			Uint32 m_id = 0u;
			HandlerSource m_source = HandlerSource::Game;
			CursorStateRequest m_cursorStateRequest = CursorStateRequest::None;
			Uint8 m_consumesInput : 1 = false;
		};

		virtual void OnInitialize() override;

		void Enable( Bool enabled )
		{
			m_enabled = enabled;
			UpdateCursorState();
		}

		forge::InputHandler RegisterHandler( Bool consumesInput, CursorStateRequest cursorStateRequest, HandlerSource source );
		void UnregisterHandler( const forge::InputHandler& handler );

		Bool IsHandlerActive( const forge::InputHandler& handler ) const;

	private:
		void UpdateCursorState();

		forge::IInput* m_input = nullptr;
		std::vector< InputHandlerDesc > m_inputHandlerDescs;

		Bool m_enabled = true;
		constexpr static Uint8 c_keysAmount = std::numeric_limits< std::underlying_type_t< forge::IInput::Key > >::max();
		std::bitset< c_keysAmount > m_keysPressed;
		std::bitset< c_keysAmount > m_keysHeld;
		std::bitset< c_keysAmount > m_keysReleased;
	};
}

namespace forge
{
	class InputHandler
	{
	public:
		using Key = forge::IInput::Key;
		using KeyState = forge::IInput::KeyState;
		using KeyEvent = forge::IInput::KeyEvent;
		using MouseButton = forge::IInput::MouseButton;
		using InputEvent = forge::IInput::InputEvent;

		InputHandler( InputHandler&& other )
			: m_id( other.m_id )
			, m_input( other.m_input )
			, m_inputSystem( other.m_inputSystem )
		{
			other.m_id = 0u;
		}

		Bool GetKey( Key key ) const { return m_inputSystem.IsHandlerActive( *this ) && m_input.GetKey( key ); }
		Bool GetKeyDown( Key key ) const { return m_inputSystem.IsHandlerActive( *this ) && m_input.GetKeyDown( key ); }
		Bool GetKeyUp( Key key ) const { return m_inputSystem.IsHandlerActive( *this ) && m_input.GetKeyUp( key ); }
		Bool GetKeys( const forge::ArraySpan< Key > keys ) const;
		Bool GetKeysUp( const forge::ArraySpan< Key > keys ) const;
		KeyState GetKeyState( Key key ) const { return m_inputSystem.IsHandlerActive( *this ) ? m_input.GetKeyState( key ) : KeyState::Released; }
		Bool GetMouseButton( MouseButton button ) const { return GetKey( forge::IInput::ConvertMouseButtonToKey( button ) ); }
		Bool GetMouseButtonDown( MouseButton button ) const { return GetKeyDown( forge::IInput::ConvertMouseButtonToKey( button ) ); }
		Bool GetMouseButtonUp( MouseButton button ) const { return GetKeyUp( forge::IInput::ConvertMouseButtonToKey( button ) ); }
		KeyState GetMouseButtonState( MouseButton button ) const { return GetKeyState( forge::IInput::ConvertMouseButtonToKey( button ) ); }
		Vector2 GetMousePosDelta() const
		{
			if ( m_inputSystem.IsHandlerActive( *this ) )
			{
				auto delta = m_input.GetMouseDeltaPos();
				return { static_cast< Float >( delta.m_x ), static_cast< Float >( delta.m_y ) };
			}

			return Vector2::ZEROS();
		}

		Float GetMouseScrollDelta() const { return m_inputSystem.IsHandlerActive( *this ) ? m_input.GetMouseScrollDelta() : 0.0f; }

	private:
		InputHandler( forge::IInput& input, systems::InputSystem& inputSystem )
			: m_input( input )
			, m_inputSystem( inputSystem )
		{
			static Uint32 s_lastHandler = 0u;
			m_id = ++s_lastHandler;
		}

		Uint32 GetId() const
		{
			return m_id;
		}

		Uint32 m_id = 0u;
		forge::IInput& m_input;
		systems::InputSystem& m_inputSystem;

		friend class systems::InputSystem;
	};
}
