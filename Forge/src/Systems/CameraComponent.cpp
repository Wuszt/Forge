#include "Fpch.h"
#include "CameraComponent.h"
#include "../Core/IWindow.h"
#include "../Renderer/PerspectiveCamera.h"
#include "TransformComponent.h"
#include "../Renderer/OrthographicCamera.h"

void forge::CameraComponent::OnAttach( EngineInstance& engineInstance )
{
	m_transformComponent = GetOwner().GetComponent< TransformComponent >();

	m_windowEventToken = engineInstance.GetWindow().RegisterEventListener( [ &, window = &engineInstance.GetWindow() ]( const forge::IWindow::IEvent& event )
	{
		if( event.GetEventType() == forge::IWindow::EventType::OnResized )
		{
			switch( GetType() )
			{
			case renderer::ICamera::Type::Perspective:
				m_implementation = std::make_unique< renderer::PerspectiveCamera >( GetDefaultPerspectiveCamera( *window ) );
				break;

			default:
				FORGE_FATAL( "Unknown type" );
				break;
			};
		}
	} );
}

void forge::CameraComponent::Update()
{
	m_implementation->SetTransform( m_transformComponent->GetData().m_transform );
}

renderer::PerspectiveCamera forge::CameraComponent::GetDefaultPerspectiveCamera( const forge::IWindow& window )
{
	return renderer::PerspectiveCamera( window.GetAspectRatio(), FORGE_PI / 3.0f, 1.0f, 10000.0f );
}

renderer::OrthographicCamera forge::CameraComponent::GetDefaultOrthographicCamera( const forge::IWindow& window )
{
	return renderer::OrthographicCamera( Vector2( 1.0f, 1.0f / window.GetAspectRatio() ) * 1000.0f, 1.0f, 10000.0f );
}
