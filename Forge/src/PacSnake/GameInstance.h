#pragma once
#include "../GameEngine/ApplicationInstance.h"

namespace pacsnake
{
	class GameInstance : public forge::ApplicationInstance
	{
	public:
		using forge::ApplicationInstance::ApplicationInstance;

		virtual void Initialize( forge::EngineInstance& engineInstance ) override;

		virtual Bool WithWindow() const override
		{
			return true;
		}
	};
}

