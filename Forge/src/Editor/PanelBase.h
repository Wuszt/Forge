#pragma once

namespace forge
{
	class EngineInstance;
}

namespace imgui
{
	class MenuBar;
}

namespace editor
{
	class PanelBase
	{
	public:
		PanelBase( Bool withMenuBar, forge::EngineInstance& engineIntance );
		virtual ~PanelBase() = default;

		void Update();

	protected:
		virtual void Draw() = 0;
		virtual const Char* GetName() const = 0;

		Vector2 GetSize() const;
		imgui::MenuBar* GetMenuBar()
		{
			return m_menuBar.get();
		}

		forge::EngineInstance& GetEngineInstance() const
		{
			return m_engineInstance;
		}

	private:
		forge::EngineInstance& m_engineInstance;
		Vector2 m_currentSize;
		std::unique_ptr< imgui::MenuBar > m_menuBar;
	};
}