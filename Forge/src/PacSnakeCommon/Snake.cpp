#include "Fpch.h"
#include "Snake.h"
#include "../Systems/RenderingComponent.h"
#include "PlayerCharacterComponent.h"
#include "../Systems/TransformComponent.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Snake );

void pacsnake::Snake::OnInit( forge::ObjectInitData& initData )
{
	Super::OnInit( initData );
	FORGE_ASSURE( AddComponent< forge::RenderingComponent >() );
	GetComponent< forge::RenderingComponent >()->LoadMeshAndMaterial( forge::Path( "PacSnake\\Snake\\Snake.fbx" ) );

	AddComponent< pacsnake::PlayerCharacterComponent >();
	GetComponent< forge::TransformComponent >()->SetWorldScale( Vector3( 2.0f, 2.0f, 2.0f ) );
}
