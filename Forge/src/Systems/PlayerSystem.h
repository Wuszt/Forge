#pragma once
#include "IDebuggable.h"
#include "../Core/IInput.h"

namespace forge
{
	class IPlayerControllerComponent;
	class InputHandler;
}

namespace systems
{
	class PlayerSystem : public ISystem, forge::IDebuggable
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PlayerSystem, systems::ISystem );

	public:
		PlayerSystem();
		PlayerSystem( PlayerSystem&& );
		~PlayerSystem();

		virtual void OnPostInit() override;
		virtual void OnDeinitialize() override;

		void SetActivePlayerComponent( forge::IPlayerControllerComponent& comp )
		{
			m_activeController = &comp;
		}

		forge::IPlayerControllerComponent* GetCurrentActiveController() const
		{
			return m_activeController;
		}

		const forge::InputHandler& GetInputHandler() const
		{
			FORGE_ASSERT( m_inputHandler );
			return *m_inputHandler;
		}

		forge::Object* GetCurrentPlayerObject() const;

	private:
		void Update();

		forge::IPlayerControllerComponent* m_activeController = nullptr;
		forge::CallbackToken m_updateToken;

		std::unique_ptr< forge::InputHandler > m_inputHandler;

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif
	};
}

