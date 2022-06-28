#pragma once
#include "ISystem.h"

namespace forge
{
	class CameraComponent;
}

namespace systems
{
	class CamerasSystem : public ISystem
	{
		DECLARE_TYPE(CamerasSystem, systems, ISystem);

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
		forge::CallbackToken m_updateToken;

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_debugOverlayToken;
#endif
	};
}

