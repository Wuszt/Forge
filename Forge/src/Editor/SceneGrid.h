#pragma once
#include "../GameEngine/ObjectLifetimeToken.h"

namespace editor
{
	class SceneGrid
	{
	public:
		SceneGrid( forge::EngineInstance& engineInstance );

	private:
		std::vector< forge::ObjectLifetimeToken > m_linesTokens;
	};
}

