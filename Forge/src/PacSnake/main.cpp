#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../GameEngine/PublicDefaults.h"

#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/SceneRenderingSystem.h"
#include "../Systems/LightingSystem.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Systems/TimeSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../GameEngine/RenderingManager.h"
#include "../Systems/InputSystem.h"

#include "../GameEngine/SceneManager.h"
#include "../Core/DepotsContainer.h"
#include "../GameEngine/SceneObject.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ISwapchain.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

Int32 main()
{
	class GameInstance : public forge::ApplicationInstance
	{
	public:
		using forge::ApplicationInstance::ApplicationInstance;

		virtual void Initialize( forge::EngineInstance& engineInstance )
		{
			const systems::ISystem::Type* systems[]
			{
				&systems::CamerasSystem::GetTypeStatic(),
				&systems::PlayerSystem::GetTypeStatic(),
				&systems::SceneRenderingSystem::GetTypeStatic(),
				&systems::LightingSystem::GetTypeStatic(),
				&systems::TimeSystem::GetTypeStatic(),
				&systems::AnimationSystem::GetTypeStatic(),
				&systems::TransformSystem::GetTypeStatic(),
				&systems::PhysicsSystem::GetTypeStatic(),
				&systems::InputSystem::GetTypeStatic(),
#ifdef FORGE_DEBUGGING
				&systems::DebugSystem::GetTypeStatic(),
#endif

#ifdef FORGE_IMGUI_ENABLED
				&systems::IMGUISystem::GetTypeStatic()
#endif
			};

			engineInstance.GetSystemsManager().AddSystems( systems );

			engineInstance.GetSystemsManager().GetSystem< systems::LightingSystem >().SetAmbientColor( { 0.55f, 0.55f, 0.55f } );
			engineInstance.GetSystemsManager().GetSystem< systems::SceneRenderingSystem >().SetTargetTexture( &engineInstance.GetRenderingManager().GetRenderer().GetSwapchain()->GetBackBuffer() );

			forge::Path mainScenePath;
			FORGE_ASSURE( engineInstance.GetDepotsContainer().TryToGetExistingFilePath( forge::Path( "PacSnake/Main.fscene" ), mainScenePath ) );
			engineInstance.GetSceneManager().LoadScene( mainScenePath );

			engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ & ]( forge::Object& player, forge::ObjectInitData& )
			{
				player.AddComponents< forge::TransformComponent, forge::CameraComponent, forge::PhysicsFreeCameraControllerComponent >();
				player.GetComponent< forge::TransformComponent >()->SetWorldPosition( { 0.0f, -10.0f, 0.0f } );
				auto* cameraComp = player.GetComponent< forge::CameraComponent >();
				cameraComp->CreateImplementation< renderer::PerspectiveCamera >( forge::CameraComponent::GetDefaultPerspectiveCamera( engineInstance.GetRenderingManager().GetWindow() ) );

				auto& camerasSystem = engineInstance.GetSystemsManager().GetSystem< systems::CamerasSystem >();
				camerasSystem.SetActiveCamera( cameraComp );

				auto* freeCameraController = player.GetComponent< forge::PhysicsFreeCameraControllerComponent >();
				engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController );
			} } );
		}

		virtual Bool WithWindow() const override
		{
			return true;
		}
	} gameInstance( "PacSnake" );

	forge::EngineInstance engineInstance( gameInstance );
	engineInstance.Run();
}