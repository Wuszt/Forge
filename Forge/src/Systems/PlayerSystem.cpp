#include "Fpch.h"
#include "PlayerSystem.h"
#include "PlayerControllerComponent.h"
#include "../Core/IInput.h"
#include "../Core/IWindow.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../IMGUI/PublicDefaults.h"
#include "CameraComponent.h"
#include "../Renderer/ICamera.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Renderer/OrthographicCamera.h"
#endif

void systems::PlayerSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, std::bind( &systems::PlayerSystem::Update, this ) );
}

forge::Entity* systems::PlayerSystem::GetCurrentPlayerEntity() const
{
	return m_activeController ? &m_activeController->GetOwner() : nullptr;
}

void systems::PlayerSystem::Update()
{
	auto input = GetEngineInstance().GetWindow().GetInput();
	if( input->GetKey( forge::IInput::Key::Shift ) && input->GetMouseButton( forge::IInput::MouseButton::MiddleButton ) )
	{
		if( !m_wasShiftAndWheelPressed )
		{
			input->LockCursor( !input->IsCursorLocked() );
		}

		m_wasShiftAndWheelPressed = true;
	}
	else
	{
		m_wasShiftAndWheelPressed = false;
	}

	if( input->IsCursorLocked() )
	{
		if( m_activeController )
		{
			m_activeController->Update();
		}
	}
}

void systems::PlayerSystem::OnRenderDebug()
{
	if( ImGui::Begin( "Player System" ) )
	{
		if( auto* cameraComponent = GetCurrentPlayerEntity()->GetComponent< forge::CameraComponent >() )
		{
			if( ImGui::TreeNodeEx( "Camera", ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				Int32 currentType = static_cast<Int32>( cameraComponent->GetType() );
				ImGui::Text( "Type: " );
				ImGui::SameLine(); ImGui::RadioButton( "Perspective", &currentType, 0 );
				ImGui::SameLine(); ImGui::RadioButton( "Orthographic", &currentType, 1 );

				auto type = static_cast< renderer::ICamera::Type >( currentType );
				if( type != cameraComponent->GetType() )
				{
					if( type == renderer::ICamera::Type::Perspective )
					{
						cameraComponent->CreateImplementation< renderer::PerspectiveCamera >( forge::CameraComponent::GetDefaultPerspectiveCamera( GetEngineInstance().GetWindow() ) );
					}
					else
					{
						cameraComponent->CreateImplementation< renderer::OrthographicCamera >( forge::CameraComponent::GetDefaultOrthographicCamera( GetEngineInstance().GetWindow() ) );
					}
				}

				constexpr Float eps = 0.001f;
				if( cameraComponent->GetType() == renderer::ICamera::Type::Perspective )
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
