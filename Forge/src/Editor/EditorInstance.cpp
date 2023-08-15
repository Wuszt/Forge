#include "Fpch.h"
#include "EditorInstance.h"
#include "../GameEngine/EngineInstance.h"
#include "../GameEngine/RenderingManager.h"
#include "../Core/IWindow.h"
#include "../Core/IInput.h"
#include "../GameEngine/ObjectsManager.h"
#include "../Systems/TransformComponent.h"
#include "../Systems/RenderingComponent.h"
#include "../Systems/PhysicsComponent.h"
#include "../GameEngine/Object.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

#ifdef FORGE_IMGUI_ENABLED
#include "../Systems/IMGUISystem.h"
#endif
#include "../GameEngine/ISystem.h"
#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/SceneRenderingSystem.h"
#include "../Systems/LightingSystem.h"
#include "../Systems/TimeSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../GameEngine/SystemsManager.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ISwapchain.h"

void CubeScene(forge::EngineInstance& engineInstance)
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >([&](forge::Object* obj)
		{
			obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsStaticComponent >([engineInstancePtr = &engineInstance, obj]()
				{
					auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
					auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

					renderingComponent->LoadMeshAndMaterial("Models\\cube.obj");

					transformComponent->GetDirtyTransform().SetPosition(Vector3::ZEROS());
				});
		});
}

void forge::EditorInstance::Initialize(forge::EngineInstance& engineInstance)
{
	const systems::ISystem::ClassType* systems[]
	{
		&systems::CamerasSystem::GetTypeStatic(),
		&systems::PlayerSystem::GetTypeStatic(),
		&systems::SceneRenderingSystem::GetTypeStatic(),
		&systems::LightingSystem::GetTypeStatic(),
		&systems::TimeSystem::GetTypeStatic(),
		&systems::AnimationSystem::GetTypeStatic(),
		&systems::TransformSystem::GetTypeStatic(),
		&systems::PhysicsSystem::GetTypeStatic(),
#ifdef FORGE_DEBUGGING
				& systems::DebugSystem::GetTypeStatic(),
#endif

#ifdef FORGE_IMGUI_ENABLED
				& systems::IMGUISystem::GetTypeStatic()
#endif
	};

	engineInstance.GetSystemsManager().AddSystems(systems);
	engineInstance.GetSystemsManager().GetSystem< systems::LightingSystem >().SetAmbientColor({ 0.55f, 0.55f, 0.55f });

	engineInstance.GetSystemsManager().GetSystem< systems::SceneRenderingSystem >().SetTargetTexture(&engineInstance.GetRenderingManager().GetRenderer().GetSwapchain()->GetBackBuffer());

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >([&](forge::Object* player)
		{
			player->RequestAddingComponents< forge::TransformComponent, forge::CameraComponent, forge::FreeCameraControllerComponent >([engineInstancePtr = &engineInstance, player]()
				{
					player->GetComponent< forge::TransformComponent >()->GetDirtyTransform().SetPosition({ 0.0f, -5.0f, 0.0f });
					auto* cameraComp = player->GetComponent< forge::CameraComponent >();
					cameraComp->CreateImplementation< renderer::PerspectiveCamera >(forge::CameraComponent::GetDefaultPerspectiveCamera(engineInstancePtr->GetRenderingManager().GetWindow()));

					auto& camerasSystem = engineInstancePtr->GetSystemsManager().GetSystem< systems::CamerasSystem >();
					camerasSystem.SetActiveCamera(cameraComp);

					auto* freeCameraController = player->GetComponent< forge::FreeCameraControllerComponent >();
					engineInstancePtr->GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent(*freeCameraController);

				});
		});

	CubeScene(engineInstance);

}

void forge::EditorInstance::OnUpdate(forge::EngineInstance& engineInstance)
{
	if (engineInstance.GetRenderingManager().GetWindow().GetInput()->GetKeyDown(forge::IInput::Key::Escape))
	{
		Shutdown();
	}
}
