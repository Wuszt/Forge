#pragma once

namespace forge
{
	class EngineInstance;
}

namespace editor
{
	class IPanel
	{
	public:
		IPanel( forge::EngineInstance& engineIntance );
		virtual ~IPanel() = default;

		virtual void Draw() = 0;

	protected:
		forge::EngineInstance& GetEngineInstance() const
		{
			return m_engineInstance;
		}

	private:
		forge::EngineInstance& m_engineInstance;
	};
}