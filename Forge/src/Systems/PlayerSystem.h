#pragma once
#include "IDebuggable.h"

namespace forge
{
	class PlayerControllerComponent;
}

namespace systems
{
	class PlayerSystem : public ISystem, forge::IDebuggable
	{
		DECLARE_CLASS( PlayerSystem, systems, ISystem );

	public:
		using ISystem::ISystem;

		virtual void OnInitialize();

		void SetActivePlayerComponent( forge::PlayerControllerComponent& comp )
		{
			m_activeController = &comp;
		}

		forge::PlayerControllerComponent* GetCurrentActiveController() const
		{
			return m_activeController;
		}

		forge::Entity* GetCurrentPlayerEntity() const;

	private:
		void Update();

		forge::PlayerControllerComponent* m_activeController = nullptr;
		forge::CallbackToken m_updateToken;

		Bool m_wasShiftAndWheelPressed = false;

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif
	};
}

