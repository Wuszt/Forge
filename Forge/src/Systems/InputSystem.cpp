#include "Fpch.h"
#include "InputSystem.h"
#include "../GameEngine/RenderingManager.h"
#include "../Core/IWindow.h"

RTTI_IMPLEMENT_TYPE( systems::InputSystem );

void systems::InputSystem::OnInitialize()
{
	m_input = &GetEngineInstance().GetRenderingManager().GetWindow().GetInput();
}

forge::InputHandler systems::InputSystem::RegisterHandler( Bool consumesInput, CursorStateRequest cursorStateRequest, HandlerSource source )
{
	FORGE_ASSERT( m_input );
	forge::InputHandler handler( *m_input, *this );

	InputHandlerDesc desc{ handler.GetId(), source, cursorStateRequest, consumesInput };

	auto it = m_inputHandlerDescs.rbegin();
	for ( ; it != m_inputHandlerDescs.rend(); ++it )
	{
		if ( static_cast< Uint8 >( it->m_source ) <= static_cast< Uint8 >( source ) )
		{
			break;
		}
	}

	m_inputHandlerDescs.insert( it.base(), desc );
	UpdateCursorState();
	return handler;
}

void systems::InputSystem::UnregisterHandler( const forge::InputHandler& handler )
{
	m_inputHandlerDescs.erase( std::remove_if( m_inputHandlerDescs.begin(), m_inputHandlerDescs.end(), [ &handler ]( const InputHandlerDesc& desc )
		{
			return desc.m_id == handler.GetId();
		} ) );

	UpdateCursorState();
}

Bool systems::InputSystem::IsHandlerActive( const forge::InputHandler& handler ) const
{
	if ( !m_enabled )
	{
		return false;
	}

	for ( auto it = m_inputHandlerDescs.rbegin(); it != m_inputHandlerDescs.rend(); ++it )
	{
		if ( it->m_id == handler.GetId() )
		{
			return true;
		}

		if ( it->m_consumesInput )
		{
			return false;
		}
	}

	return false;
}

void systems::InputSystem::UpdateCursorState()
{
	if ( m_enabled )
	{
		for ( auto it = m_inputHandlerDescs.rbegin(); it != m_inputHandlerDescs.rend(); ++it )
		{
			if ( it->m_cursorStateRequest != CursorStateRequest::None )
			{
				m_input->LockCursor( it->m_cursorStateRequest == CursorStateRequest::Disabled );
				return;
			}
		}
	}

	m_input->LockCursor( false );
}

Bool forge::InputHandler::GetKeys( const forge::ArraySpan< Key > keys ) const
{
	Bool result = m_inputSystem.IsHandlerActive( *this ) && !keys.IsEmpty();
	for ( Key key : keys )
	{
		result &= GetKey( key );
	}

	return result;
}

Bool forge::InputHandler::GetKeysUp( const forge::ArraySpan< Key > keys ) const
{
	Bool result = m_inputSystem.IsHandlerActive( *this ) && !keys.IsEmpty();
	for ( Key key : keys )
	{
		result &= GetKeyUp( key );
	}

	return result;
}
