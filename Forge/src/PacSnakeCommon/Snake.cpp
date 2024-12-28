#include "Fpch.h"
#include "Snake.h"
#include "../Systems/RenderingComponent.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Snake );

void pacsnake::Snake::OnInit( forge::ObjectInitData& initData )
{
	Super::OnInit( initData );
	FORGE_ASSURE( AddComponent< forge::RenderingComponent >() );
	GetComponent< forge::RenderingComponent >()->LoadMeshAndMaterial( forge::Path( "PacSnake\\Snake\\Snake.fbx" ) );
}
