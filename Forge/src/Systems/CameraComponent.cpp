#include "Fpch.h"
#include "CameraComponent.h"
#include "../Core/IWindow.h"
#include "../Renderer/PerspectiveCamera.h"
#include "TransformComponent.h"

void forge::CameraComponent::OnAttach( EngineInstance& engineInstance )
{
	m_transformComponent = GetOwner().GetComponent< TransformComponent >();

	m_windowEventToken = std::make_unique< CallbackToken >( engineInstance.GetWindow().RegisterEventListener( [ &, window = &engineInstance.GetWindow() ]( const forge::IWindow::IEvent& event )
	{
		if( event.GetEventType() == forge::IWindow::EventType::OnWindowResized )
		{
			auto prevCamera = std::move( m_implementation );

			switch( GetType() )
			{
			case ICamera::Type::Perspective:
				m_implementation = std::make_unique< forge::PerspectiveCamera >( window->GetAspectRatio(), FORGE_PI / 3.0f, 0.1f, 2000.0f );

			default:
				FORGE_FATAL( "Unknown type" );
				break;
			};
		}
	} ) );
}

void forge::CameraComponent::Update()
{
	m_implementation->SetTransform( m_transformComponent->GetData().m_transform );
}
