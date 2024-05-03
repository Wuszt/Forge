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
#include "../GameEngine/UpdateManager.h"
#include "../../External/imgui/imgui.h"
#include "SceneEditor.h"
#include "WindowBase.h"
#include "../Systems/InputSystem.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

#ifdef FORGE_IMGUI_ENABLED
#include "../Systems/IMGUISystem.h"
#endif

forge::EditorInstance::EditorInstance( const std::string& applicationName )
	: forge::ApplicationInstance( applicationName )
{}

forge::EditorInstance::~EditorInstance() = default;

void forge::EditorInstance::Initialize(forge::EngineInstance& engineInstance)
{
	m_engineInstance = &engineInstance;
	m_updateToken = engineInstance.GetUpdateManager().RegisterUpdateFunction(forge::UpdateManager::BucketType::Update, [this](){ Update(); });

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
	engineInstance.GetSystemsManager().GetSystem< systems::LightingSystem >().SetAmbientColor({ 0.55f, 0.55f, 0.55f });

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >([&]( forge::Object* player )
	{
		player->SetName( "Player" );
		player->AddComponents< forge::TransformComponent, forge::CameraComponent, forge::PhysicsFreeCameraControllerComponent >();
		player->GetComponent< forge::TransformComponent >()->SetWorldPosition({ 0.0f, -5.0f, 0.0f });
		auto* cameraComp = player->GetComponent< forge::CameraComponent >();
		cameraComp->CreateImplementation< renderer::PerspectiveCamera >(forge::CameraComponent::GetDefaultPerspectiveCamera( engineInstance.GetRenderingManager().GetWindow() ) );

		auto& camerasSystem = engineInstance.GetSystemsManager().GetSystem< systems::CamerasSystem >();
		camerasSystem.SetActiveCamera(cameraComp);

		auto* freeCameraController = player->GetComponent< forge::PhysicsFreeCameraControllerComponent >();
		engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController);
	});

	m_windows.emplace_back( std::make_unique< editor::SceneEditor >( nullptr, engineInstance ) );
}

void forge::EditorInstance::Deinitialize( EngineInstance& engineInstance )
{
	m_updateToken.Unregister();
	m_windows.clear();
}

void forge::EditorInstance::Update()
{
	if ( m_engineInstance->GetRenderingManager().GetWindow().GetInput().GetKeyDown( forge::IInput::Key::Escape ) )
	{
		Shutdown();
	}

	ImGui::DockSpaceOverViewport( ImGui::GetMainViewport() );

	for ( auto& window : m_windows )
	{
		window->Update();
	}
}