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
	class WindowBase
	{
	public:
		WindowBase( forge::EngineInstance& engineInstance, WindowBase* parent, Bool withMenuBar );
		virtual ~WindowBase();

		void Update();

		WindowBase* GetParent()
		{
			return m_parent;
		}

		forge::EngineInstance& GetEngineInstance() const
		{
			return m_engineInstance;
		}

	protected:
		virtual void Draw() = 0;
		virtual const Char* GetName() const = 0;

		template< class T , class... TArgs >
		void AddChild( TArgs&&... args )
		{
			m_children.emplace_back( std::make_unique< T >( std::forward< TArgs >( args )... ) );
		}

		Vector2 GetSize() const;
		imgui::MenuBar* GetMenuBar()
		{
			return m_menuBar.get();
		}

	private:
		forge::EngineInstance& m_engineInstance;
		Vector2 m_currentSize;
		std::unique_ptr< imgui::MenuBar > m_menuBar;

		WindowBase* m_parent = nullptr;
		std::vector< std::unique_ptr< WindowBase > > m_children;
	};
}