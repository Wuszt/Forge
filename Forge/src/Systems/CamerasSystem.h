#pragma once

namespace forge
{
	class CameraComponent;
}

namespace systems
{
	class CamerasSystem : public ISystem
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( CamerasSystem, systems::ISystem );

	public:
		using ISystem::ISystem;

		virtual void OnInitialize() override;
		virtual void OnPostInit() override;

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

