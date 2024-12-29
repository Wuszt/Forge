#include "Fpch.h"
#include "GameInstance.h"

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
#include "../Renderer/ISwapchain.h"
#include "../PacSnakeCommon/GridSystem.h"
#include "../PacSnakeCommon/Snake.h"
#include "../PacSnakeCommon/PlayerCharacterComponent.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

void pacsnake::GameInstance::Initialize( forge::EngineInstance& engineInstance )
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
		&pacsnake::GridSystem::GetTypeStatic(),

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

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ & ]( forge::Object& camera, forge::ObjectInitData& )
	{
		camera.AddComponents< forge::TransformComponent, forge::CameraComponent >();
		auto* cameraComp = camera.GetComponent< forge::CameraComponent >();
		cameraComp->CreateImplementation< renderer::PerspectiveCamera >( forge::CameraComponent::GetDefaultPerspectiveCamera( engineInstance.GetRenderingManager().GetWindow() ) );

		auto& camerasSystem = engineInstance.GetSystemsManager().GetSystem< systems::CamerasSystem >();
		camerasSystem.SetActiveCamera( cameraComp );

		auto* transformComp = camera.GetComponent< forge::TransformComponent >();
		transformComp->SetWorldPosition( { 0.0f, -4.5f, 22.0f } );
		transformComp->SetWorldOrientation( Quaternion::CreateFromDirection( -transformComp->GetWorldPosition().Normalized() ) );
	} } );

	engineInstance.GetObjectsManager().RequestCreatingObject< pacsnake::Snake >( { .m_postInitFunc = [ & ]( forge::Object& snake, forge::ObjectInitData& )
		{
			auto* playerComponent = snake.GetComponent< pacsnake::PlayerCharacterComponent >();
			engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *playerComponent );
		} } );
}