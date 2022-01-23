#pragma once
#include "ISystem.h"

namespace forge
{
	class CameraComponent;;
}

namespace systems
{
	class CamerasSystem : public ISystem
	{
	public:
		using ISystem::ISystem;

		virtual void OnInitialize() override;

		void SetActiveCamera( forge::CameraComponent* camera )
		{
			m_activeCamera = camera;
		}

		void Update();

		const forge::CameraComponent* GetActiveCamera() const
		{
			return m_activeCamera;
		}

	private:
		forge::CameraComponent* m_activeCamera = nullptr;
		std::unique_ptr< forge::CallbackToken > m_updateToken;

#ifdef FORGE_IMGUI_ENABLED
		std::unique_ptr< forge::CallbackToken > m_debugOverlayToken;
#endif
	};
}

