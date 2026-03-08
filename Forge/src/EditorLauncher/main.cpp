#include "../Core/PublicDefaults.h"
#include "../Editor/EditorInstance.h"
#include "../GameEngine/EngineInstance.h"

Int32 main( int argc, char* argv[] )
{
	forge::EditorInstance editorInstance( "Editor", forge::ApplicationArgs( argc, argv ) );
	forge::EngineInstance engineInstance( editorInstance );

	engineInstance.Run();
}