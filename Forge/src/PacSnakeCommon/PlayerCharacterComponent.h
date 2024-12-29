#pragma once
#include "../Systems/PlayerControllerComponent.h"

namespace ecs
{
	class CommandsQueue;
}

namespace forge
{
	class EngineInstance;
	class ObjectInitData;
}

namespace pacsnake
{
	class PlayerCharacterComponent : public forge::IPlayerControllerComponent
	{
		RTTI_DECLARE_CLASS( PlayerCharacterComponent, forge::IPlayerControllerComponent );
	protected:
		virtual void Update() override;
	};
}