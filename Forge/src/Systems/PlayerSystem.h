#pragma once
#include "IDebuggable.h"

namespace forge
{
	class IPlayerControllerComponent;
}

namespace systems
{
	class PlayerSystem : public ISystem, forge::IDebuggable
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PlayerSystem, systems::ISystem );

	public:
		using ISystem::ISystem;

		virtual void OnInitialize();

		void SetActivePlayerComponent( forge::IPlayerControllerComponent& comp )
		{
			m_activeController = &comp;
		}

		forge::IPlayerControllerComponent* GetCurrentActiveController() const
		{
			return m_activeController;
		}

		forge::Object* GetCurrentPlayerObject() const;

	private:
		void Update();

		forge::IPlayerControllerComponent* m_activeController = nullptr;
		forge::CallbackToken m_updateToken;

		Bool m_wasShiftAndWheelPressed = false;

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif
	};
}

