#pragma once
#include "../Renderer/ICamera.h"

namespace renderer
{
	class PerspectiveCamera;
	class OrthographicCamera;
}

namespace forge
{
	class ICamera;
	class TransformComponent;
	class IWindow;

	class CameraComponent : public IComponent
	{
		DECLARE_POLYMORPHIC_CLASS(CameraComponent, forge::IComponent );
	public:
		using IComponent::IComponent;

		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;

		renderer::ICamera::CameraType GetCameraType() const
		{
			return m_implementation->GetCameraType();
		}

		const renderer::ICamera& GetCamera() const
		{
			return *m_implementation;
		}

		renderer::ICamera& GetCamera()
		{
			return *m_implementation;
		}

		void Update();

		template< class T, class... TArgs >
		void CreateImplementation( TArgs... args )
		{
			m_implementation = std::make_unique< T >( std::forward< TArgs >( args )... );
		}

		static renderer::PerspectiveCamera GetDefaultPerspectiveCamera( const forge::IWindow& window );
		static renderer::OrthographicCamera GetDefaultOrthographicCamera( const forge::IWindow& window );

	private:
		std::unique_ptr< renderer::ICamera > m_implementation;
		forge::CallbackToken m_windowEventToken;
		TransformComponent* m_transformComponent;
	};
}

