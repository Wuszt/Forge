#pragma once

#ifdef FORGE_IMGUI_ENABLED
namespace imgui
{
	class MenuBarItem;

	class MenuBar
	{
	public:
		class Element
		{
		public:
			Element( const char* name, std::shared_ptr< Element > parent )
				: m_name( name )
				, m_parent( parent )
			{}

			virtual ~Element() = default;

			const char* GetName() const
			{
				return m_name;
			}

		private:
			const char* m_name;
			std::shared_ptr< Element > m_parent;
		};

		class Menu : public Element
		{
		public:
			using Element::Element;
			std::vector< std::weak_ptr< Element > >& GetChildren()
			{
				return m_children;
			}
		private:
			std::vector< std::weak_ptr< Element > > m_children;
		};

		MenuBar( Bool isMainMenu = false )
			: m_isMainMenu( isMainMenu )
		{}

		using ItemWeakHandle = std::weak_ptr< MenuBarItem >;

		std::shared_ptr< MenuBarItem > AddButton( forge::ArraySpan< const char* > path, Bool selectable );
		void Draw();

	private:
		std::vector< std::weak_ptr< Element > > m_rootElements;
		Bool m_isMainMenu = false;
	};

	class MenuBarItem : public MenuBar::Element
	{
	public:
		MenuBarItem( const char* name, std::shared_ptr< MenuBar::Menu > parent, Bool selectable )
			: Element( name, parent )
		{
			if ( selectable )
			{
				m_onClickedToken = m_onClicked.AddListener( [ this ]()
					{
						m_selected = !m_selected;
					} );
			}
		}

		Bool IsSelected() const
		{
			return m_selected;
		}

		void SetSelected( Bool value )
		{
			m_selected = value;
		}

		forge::Callback<>& GetCallback()
		{
			return m_onClicked;
		}

	private:
		forge::Callback<> m_onClicked;
		forge::CallbackToken m_onClickedToken;
		Bool m_selected = false;
	};

	using MenuBarItemHandle = std::shared_ptr< MenuBarItem >;
}
#endif