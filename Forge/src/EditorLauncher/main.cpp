#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../GameEngine/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/RenderingSystem.h"
#include "../Core/IInput.h"
#include "../Systems/LightingSystem.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Systems/TimeSystem.h"
#include "../Renderer/SkeletonAsset.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

#ifdef FORGE_IMGUI_ENABLED
#endif
#include "../Systems/AnimationSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/PhysicsComponent.h"
#include "../Renderer/Model.h"

void CubeScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
		{
			obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsStaticComponent >( [ engineInstancePtr = &engineInstance, obj ]()
				{
					auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
					auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

					renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );

					transformComponent->GetDirtyTransform().SetPosition( Vector3::ZEROS() );
				} );
		} );
}

Int32 main()
{
	class EditorInstance : public forge::ApplicationInstance
	{
	public:
		using forge::ApplicationInstance::ApplicationInstance;

		virtual void Initialize( forge::EngineInstance& engineInstance )
		{
			const systems::ISystem::ClassType* systems[]
			{
				&systems::CamerasSystem::GetTypeStatic(),
				&systems::PlayerSystem::GetTypeStatic(),
				&systems::RenderingSystem::GetTypeStatic(),
				&systems::LightingSystem::GetTypeStatic(),
				&systems::TimeSystem::GetTypeStatic(),
				&systems::AnimationSystem::GetTypeStatic(),
				&systems::TransformSystem::GetTypeStatic(),
				&systems::PhysicsSystem::GetTypeStatic(),
#ifdef FORGE_DEBUGGING
				&systems::DebugSystem::GetTypeStatic(),
#endif

#ifdef FORGE_IMGUI_ENABLED
				&systems::IMGUISystem::GetTypeStatic()
#endif
			};

			engineInstance.GetSystemsManager().AddSystems( systems );
			engineInstance.GetSystemsManager().GetSystem< systems::LightingSystem >().SetAmbientColor( { 0.55f, 0.55f, 0.55f } );

			engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* player )
				{
					player->RequestAddingComponents< forge::TransformComponent, forge::CameraComponent, forge::FreeCameraControllerComponent >( [ engineInstancePtr = &engineInstance, player ]()
						{
							player->GetComponent< forge::TransformComponent >()->GetDirtyTransform().SetPosition( { 0.0f, -5.0f, 0.0f } );
							auto* cameraComp = player->GetComponent< forge::CameraComponent >();
							cameraComp->CreateImplementation< renderer::PerspectiveCamera >( forge::CameraComponent::GetDefaultPerspectiveCamera( engineInstancePtr->GetWindow() ) );

							auto& camerasSystem = engineInstancePtr->GetSystemsManager().GetSystem< systems::CamerasSystem >();
							camerasSystem.SetActiveCamera( cameraComp );

							auto* freeCameraController = player->GetComponent< forge::FreeCameraControllerComponent >();
							engineInstancePtr->GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController );

						} );
				} );

			CubeScene(engineInstance);
		}

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{
			if ( engineInstance.GetWindow().GetInput()->GetKeyDown( forge::IInput::Key::Escape ) )
			{
				Shutdown();
			}
		}

		virtual Bool WithRendering() const override
		{
			return true;
		}

	} editorInstance( "Editor" );

	forge::EngineInstance engineInstance( editorInstance );

	engineInstance.Run();
}