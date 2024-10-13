#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../GameEngine/PublicDefaults.h"
#include "../Editor/EditorInstance.h"
#include "../Systems/TransformComponent.h"
#include "../Systems/RenderingComponent.h"
#include "../Systems/PhysicsComponent.h"
#include "../Renderer/ModelAsset.h"
#include "../Renderer/IVertexBuffer.h"
#include "../Core/AssetsManager.h"
#include "../Renderer/Model.h"
#include "../Physics/PhysicsShape.h"
#include "../Systems/PhysicsSystem.h"
#include "../GameEngine/SceneObject.h"

Int32 main()
{
	forge::EditorInstance editorInstance( "Editor" );
	forge::EngineInstance engineInstance( editorInstance );

	engineInstance.Run();
}