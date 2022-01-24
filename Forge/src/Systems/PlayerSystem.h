#pragma once
#include "ISystem.h"

namespace forge
{
	class PlayerControllerComponent;
}

namespace systems
{

	class PlayerSystem : public ISystem
	{
	public:
		using ISystem::ISystem;

		virtual void OnInitialize();

		FORGE_INLINE void SetActivePlayerComponent( forge::PlayerControllerComponent& comp )
		{
			m_activeController = &comp;
		}

		FORGE_INLINE forge::PlayerControllerComponent* GetCurrentActiveController() const
		{
			return m_activeController;
		}

		forge::Entity* GetCurrentPlayerEntity() const;

	private:
		void Update();

		forge::PlayerControllerComponent* m_activeController = nullptr;
		forge::CallbackToken m_updateToken;

		Bool m_wasShiftAndWheelPressed = false;
	};
}

