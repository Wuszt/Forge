#pragma once
#include "../Renderer/ICamera.h"

namespace forge
{
	class ICamera;
	class TransformComponent;

	class CameraComponent : public IComponent
	{
	public:
		using IComponent::IComponent;

		virtual void OnAttach( EngineInstance& engineInstance ) override;

		FORGE_INLINE renderer::ICamera::Type GetType() const
		{
			return m_implementation->GetType();
		}

		FORGE_INLINE const renderer::ICamera& GetCamera() const
		{
			return *m_implementation;
		}

		void Update();

		template< class T, class... TArgs >
		void CreateImplementation( TArgs... args )
		{
			m_implementation = std::make_unique< T >( std::forward< TArgs >( args )... );
		}

	private:
		std::unique_ptr< renderer::ICamera > m_implementation;
		forge::CallbackToken m_windowEventToken;
		TransformComponent* m_transformComponent;
	};
}

