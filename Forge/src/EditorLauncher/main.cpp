#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../GameEngine/PublicDefaults.h"
#include "../Editor/EditorInstance.h"

Int32 main()
{
	forge::EditorInstance editorInstance("Editor");
	forge::EngineInstance engineInstance( editorInstance );

	engineInstance.Run();
}