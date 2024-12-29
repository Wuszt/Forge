#include "Fpch.h"
#include "Character.h"
#include "../PacSnakeCommon/GridSystem.h"
#include "../Systems/TransformComponent.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Character );

void pacsnake::Character::TurnLeft()
{
	TurnRight();
	TurnRight();
	TurnRight();
}

void pacsnake::Character::TurnRight()
{
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	GridPawn* pawn = gridSystem.GetGrid().GetPawn( m_pawnID );
	if ( pawn->m_dir == Vector2( 1.0f, 0.0f ) )
	{
		pawn->m_dir = Vector2( 0.0f, -1.0f );
	}
	else if ( pawn->m_dir == Vector2( 0.0f, -1.0f ) )
	{
		pawn->m_dir = Vector2( -1.0f, 0.0f );
	}
	else if ( pawn->m_dir == Vector2( -1.0f, 0.0f ) )
	{
		pawn->m_dir = Vector2( 0.0f, 1.0f );
	}
	else if ( pawn->m_dir == Vector2( 0.0f, 1.0f ) )
	{
		pawn->m_dir = Vector2( 1.0f, 0.0f );
	}
}

const Vector2& pacsnake::Character::GetDir() const
{
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	GridPawn* pawn = gridSystem.GetGrid().GetPawn( m_pawnID );
	return pawn->m_dir;
}

void pacsnake::Character::OnInit( forge::ObjectInitData& initData )
{
	Super::OnInit( initData );
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();

	m_beforeGridUpdateToken = gridSystem.RegisterOnBeforeSimUpdate( [ this, &gridSystem ]()
		{
			switch ( m_scheduledAction )
			{
			case Action::None:
				break;

			case Action::TurnLeft:
				TurnLeft();
				break;

			case Action::TurnRight:
				TurnRight();
				break;
			default:
				FORGE_ASSERT( false );
			}

			m_scheduledAction = Action::None;

			m_prevPeriod = gridSystem.GetPeriod();
			GridPawn* pawn = gridSystem.GetGrid().GetPawn( m_pawnID );

			m_prevPos = pawn->m_pos;
		} );

	m_gridUpdateToken = gridSystem.RegisterOnSimUpdate( [ this ]()
		{
			auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
			GridPawn* pawn = gridSystem.GetGrid().GetPawn( m_pawnID );

			auto* transformComp = GetComponent< forge::TransformComponent >();
			transformComp->SetWorldOrientation( Quaternion::CreateFromDirection( Vector3( pawn->m_dir, 0.0f ) ) );
		} );

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update,
		[ this ]()
		{
			auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
			GridPawn* pawn = gridSystem.GetGrid().GetPawn( m_pawnID );

			const Float currentTime = forge::Time::GetTime();
			const Float t = ( currentTime - gridSystem.GetLastSimUpdateTime() ) / m_prevPeriod;

			GetComponent< forge::TransformComponent >()->SetWorldPosition( Vector3( Math::Lerp( m_prevPos, pawn->m_pos, t ), 0.5f ) );
		} );

	m_pawnID = gridSystem.GetGrid().AddPawn( Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 0.0f ) );
}
