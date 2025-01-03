#include "Fpch.h"
#include "PlayerSystem.h"
#include "PlayerControllerComponent.h"
#include "../Core/IInput.h"
#include "../Core/IWindow.h"
#include "../GameEngine/RenderingManager.h"
#include "../Systems/InputSystem.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../IMGUI/PublicDefaults.h"
#include "CameraComponent.h"
#include "../Renderer/ICamera.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Renderer/OrthographicCamera.h"
#endif

RTTI_IMPLEMENT_TYPE( systems::PlayerSystem );

systems::PlayerSystem::PlayerSystem() = default;

systems::PlayerSystem::PlayerSystem( PlayerSystem&& ) = default;

systems::PlayerSystem::~PlayerSystem() = default;

void systems::PlayerSystem::OnPostInit()
{
#ifdef FORGE_IMGUI_ENABLED
	InitializeDebuggable< systems::PlayerSystem >( GetEngineInstance() );
#endif

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, std::bind( &systems::PlayerSystem::Update, this ) );
	auto& inputSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::InputSystem >();
	m_inputHandler = std::make_unique< forge::InputHandler >( inputSystem.RegisterHandler( false, InputSystem::CursorStateRequest::Disabled, InputSystem::HandlerSource::Game ) );
}

void systems::PlayerSystem::OnDeinitialize()
{
	auto& inputSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::InputSystem >();
	inputSystem.UnregisterHandler( *m_inputHandler );
}

forge::Object* systems::PlayerSystem::GetCurrentPlayerObject() const
{
	return m_activeController ? &m_activeController->GetOwner() : nullptr;
}

void systems::PlayerSystem::Update()
{
	if ( m_activeController )
	{
		m_activeController->Update();
	}
}

#ifdef FORGE_IMGUI_ENABLED
void systems::PlayerSystem::OnRenderDebug()
{
	if( ImGui::Begin( "Player System" ) )
	{
		if( auto* cameraComponent = GetCurrentPlayerObject()->GetComponent< forge::CameraComponent >() )
		{
			if( ImGui::TreeNodeEx( "Camera", ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				Int32 currentType = static_cast<Int32>( cameraComponent->GetCameraType() );
				ImGui::Text( "Type: " );
				ImGui::SameLine(); ImGui::RadioButton( "Perspective", &currentType, 0 );
				ImGui::SameLine(); ImGui::RadioButton( "Orthographic", &currentType, 1 );

				auto type = static_cast< renderer::ICamera::CameraType >( currentType );
				if( type != cameraComponent->GetCameraType() )
				{
					if( type == renderer::ICamera::CameraType::Perspective )
					{
						cameraComponent->CreateImplementation< renderer::PerspectiveCamera >( forge::CameraComponent::GetDefaultPerspectiveCamera( GetEngineInstance().GetRenderingManager().GetWindow() ) );
					}
					else
					{
						cameraComponent->CreateImplementation< renderer::OrthographicCamera >( forge::CameraComponent::GetDefaultOrthographicCamera( GetEngineInstance().GetRenderingManager().GetWindow() ) );
					}
				}

				constexpr Float eps = 0.001f;
				if( cameraComponent->GetCameraType() == renderer::ICamera::CameraType::Perspective )
				{
					renderer::PerspectiveCamera& camera = static_cast< renderer::PerspectiveCamera& >( cameraComponent->GetCamera() );
					{
						Float nearPlane = camera.GetNearPlane();
						ImGui::SliderFloat( "Near Plane", &nearPlane, eps, camera.GetFarPlane() - eps );
						camera.SetNearPlane( nearPlane );
					}

					{
						Float farPlane = camera.GetFarPlane();
						ImGui::SliderFloat( "Far Plane", &farPlane, camera.GetNearPlane() + eps, 100000.0f );
						camera.SetFarPlane( farPlane );
					}

					{
						Float fov = camera.GetFOV();
						ImGui::SliderFloat( "FOV", &fov, eps, FORGE_PI - eps);
						camera.SetFOV( fov );
					}

					{
						Float aspectRatio = camera.GetAspectRatio();
						ImGui::SliderFloat( "Aspect Ratio", &aspectRatio, eps, 1.0f / eps );
						camera.SetAspectRatio( aspectRatio );
					}
				}
				else
				{
					renderer::OrthographicCamera& camera = static_cast<renderer::OrthographicCamera&>( cameraComponent->GetCamera() );
					{
						Float nearPlane = camera.GetNearPlane();
						ImGui::SliderFloat( "Near Plane", &nearPlane, eps, camera.GetFarPlane() - eps );
						camera.SetNearPlane( nearPlane );
					}

					{
						Float farPlane = camera.GetFarPlane();
						ImGui::SliderFloat( "Far Plane", &farPlane, camera.GetNearPlane() + eps, 100000.0f );
						camera.SetFarPlane( farPlane );
					}

					{
						Float width = camera.GetVolumeSize().X;
						ImGui::SliderFloat( "Width", &width, 1.0f, 10000.0f );
						camera.SetWidth( width );
					}
				}

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}
#endif
