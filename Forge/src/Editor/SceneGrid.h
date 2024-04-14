#pragma once

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

