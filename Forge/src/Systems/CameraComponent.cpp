#include "Fpch.h"
#include "CameraComponent.h"
#include "../Core/IWindow.h"
#include "../Renderer/PerspectiveCamera.h"
#include "TransformComponent.h"
#include "../Renderer/OrthographicCamera.h"
#include "../GameEngine/RenderingManager.h"
#include "SceneRenderingSystem.h"

RTTI_IMPLEMENT_TYPE( forge::CameraComponent );

void forge::CameraComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData )
{
	Super::OnAttached( engineInstance, commandsQueue, initData );

	m_transformComponent = GetOwner().GetComponent< TransformComponent >();

	m_onResolutionChangeToken = engineInstance.GetRenderingManager().GetWindow().RegisterEventListener( [ &, window = &engineInstance.GetRenderingManager().GetWindow() ]( const forge::IWindow::IEvent& event )
	{
		if( event.GetEventType() == forge::IWindow::EventType::OnResized )
		{
			switch( GetCameraType() )
			{
			case renderer::ICamera::CameraType::Perspective:
			{
				auto& camera = static_cast<renderer::PerspectiveCamera&>( *m_implementation );
				m_implementation = std::make_unique< renderer::PerspectiveCamera >( window->GetAspectRatio(), camera.GetFOV(), camera.GetNearPlane(), camera.GetFarPlane() );
				break;
			}
			case renderer::ICamera::CameraType::Orthographic:
			{
				auto& camera = static_cast<renderer::OrthographicCamera&>( *m_implementation );
				m_implementation = std::make_unique< renderer::OrthographicCamera >( camera.GetVolumeSize().X, window->GetAspectRatio(), camera.GetNearPlane(), camera.GetFarPlane() );
				break;
			}

			default:
				FORGE_FATAL( "Unknown type" );
				break;
			};
		}
	} );

	m_onResolutionChangeToken = engineInstance.GetSystemsManager().GetSystem< systems::SceneRenderingSystem >().RegisterOnRenderingResolutionChange( [ this ]( const Vector2& res )
	{
			const Float aspectRatio = res.X / res.Y;

			switch ( GetCameraType() )
			{
			case renderer::ICamera::CameraType::Perspective:
			{
				auto& camera = static_cast< renderer::PerspectiveCamera& >( *m_implementation );
				m_implementation = std::make_unique< renderer::PerspectiveCamera >( aspectRatio, camera.GetFOV(), camera.GetNearPlane(), camera.GetFarPlane() );
				break;
			}
			case renderer::ICamera::CameraType::Orthographic:
			{
				auto& camera = static_cast< renderer::OrthographicCamera& >( *m_implementation );
				m_implementation = std::make_unique< renderer::OrthographicCamera >( camera.GetVolumeSize().X, aspectRatio, camera.GetNearPlane(), camera.GetFarPlane() );
				break;
			}

			default:
				FORGE_FATAL( "Unknown type" );
				break;
			};
	} );
}

void forge::CameraComponent::Update()
{
	m_implementation->SetTransform( m_transformComponent->GetWorldTransform() );
}

renderer::PerspectiveCamera forge::CameraComponent::GetDefaultPerspectiveCamera( const forge::IWindow& window )
{
	return renderer::PerspectiveCamera( window.GetAspectRatio(), FORGE_PI / 3.0f, 1.0f, 10000.0f );
}

renderer::OrthographicCamera forge::CameraComponent::GetDefaultOrthographicCamera( const forge::IWindow& window )
{
	return renderer::OrthographicCamera( 1000.0f, window.GetAspectRatio(), 1.0f, 10000.0f );
}
