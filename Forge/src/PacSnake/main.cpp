#include "Fpch.h"
#include "GameInstance.h"
#include "../GameEngine/EngineInstance.h"

int main()
{
	pacsnake::GameInstance gameInstance( "PacSnake" );

	forge::EngineInstance engineInstance( gameInstance );
	engineInstance.Run();
}