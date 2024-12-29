#include "Fpch.h"
#include "PlayerCharacterComponent.h"
#include "Character.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/InputSystem.h"

RTTI_IMPLEMENT_TYPE( pacsnake::PlayerCharacterComponent );

pacsnake::Character::Action DirToAction( const Vector2& currentDir, const Vector2& newDir )
{
	const Float crossProduct = Vector3( currentDir, 0.0f ).Cross( Vector3( newDir, 0.0f ) ).Z;

	if ( crossProduct > 0.0f )
	{
		return pacsnake::Character::Action::TurnLeft;
	}
	else if( crossProduct < 0.0f )
	{
		return pacsnake::Character::Action::TurnRight;
	}

	return pacsnake::Character::Action::None;
}

void pacsnake::PlayerCharacterComponent::Update()
{
	auto& playerSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< systems::PlayerSystem >();
	const auto& inputHandler = playerSystem.GetInputHandler();

	pacsnake::Character& character = rtti::CastChecked< pacsnake::Character >( GetOwner() );

	Vector2 dir;
	if ( inputHandler.GetKeyDown( forge::InputHandler::Key::LeftArrow ) )
	{
		dir = Vector2( -1.0f, 0.0f );
	}
	else if ( inputHandler.GetKeyDown( forge::InputHandler::Key::RightArrow ) )
	{
		dir = Vector2( 1.0f, 0.0f );
	}
	else if ( inputHandler.GetKeyDown( forge::InputHandler::Key::UpArrow ) )
	{
		dir = Vector2( 0.0f, 1.0f );
	}
	else if ( inputHandler.GetKeyDown( forge::InputHandler::Key::DownArrow ) )
	{
		dir = Vector2( 0.0f, -1.0f );
	}

	pacsnake::Character::Action action = DirToAction( character.GetDir(), dir );
	if ( action != pacsnake::Character::Action::None )
	{
		character.ScheduleAction( action );
	}
}
