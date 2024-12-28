#include "../Core/PublicDefaults.h"
#include "../Editor/EditorInstance.h"
#include "../GameEngine/EngineInstance.h"
#include "../PacSnakeCommon/PublicDefaults.h"

Int32 main()
{
	forge::EditorInstance editorInstance( "PacSnake" );
	forge::EngineInstance engineInstance( editorInstance );

	engineInstance.Run();
}