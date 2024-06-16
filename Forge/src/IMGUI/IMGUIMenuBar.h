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

		std::shared_ptr< MenuBarItem > AddButton( forge::ArraySpan< const char* > path, std::function<void()> onClickedFunc, Bool selectable = false );
		void Draw();

	private:
		std::vector< std::weak_ptr< Element > > m_rootElements;
		Bool m_isMainMenu = false;
	};

	class MenuBarItem : public MenuBar::Element
	{
	public:
		MenuBarItem( const char* name, std::shared_ptr< MenuBar::Menu > parent, forge::Callback<>::TFunc onClickedFunc, Bool selectable = false )
			: Element( name, parent )
		{
			if ( selectable )
			{
				m_updateStateToken = m_onClicked.AddListener( [ this ]()
					{
						m_selected = !m_selected;
					} );
			}

			SetOnClicked( onClickedFunc );
		}

		Bool IsSelected() const
		{
			return m_selected;
		}

		void SetSelected( Bool value )
		{
			m_selected = value;
		}

		void SetOnClicked( forge::Callback<>::TFunc func )
		{
			m_userFuncToken = m_onClicked.AddListener( std::move( func ) );
		}

		forge::Callback<>& GetCallback()
		{
			return m_onClicked;
		}

	private:
		forge::Callback<> m_onClicked;
		forge::CallbackToken m_updateStateToken;
		forge::CallbackToken m_userFuncToken;
		Bool m_selected = false;
	};

	using MenuBarItemHandle = std::shared_ptr< MenuBarItem >;
}
#endif