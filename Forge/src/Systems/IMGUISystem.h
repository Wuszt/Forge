#pragma once

#ifdef FORGE_IMGUI_ENABLED

namespace forge
{
	class IMGUIInstance;
}

namespace imgui
{
	class TopBarItem;

	class TopBar
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
			std::shared_ptr<Element> m_parent;
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

		using ItemWeakHandle = std::weak_ptr< TopBarItem >;

		std::shared_ptr< TopBarItem > AddButton( forge::ArraySpan< const char* > path, Bool selectable );
		void Draw();

	private:
		std::vector< std::weak_ptr< Element > > m_rootElements;
	};

	class TopBarItem : public TopBar::Element
	{
	public:
		TopBarItem( const char* name, std::shared_ptr<TopBar::Menu> parent, Bool selectable )
			: Element( name, parent )
		{
			if( selectable )
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

	using TopBarItemHandle = std::shared_ptr< TopBarItem >;
}

namespace systems
{
	class IMGUISystem : public ISystem
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( IMGUISystem, systems::ISystem );
	public:
		IMGUISystem();
		IMGUISystem(IMGUISystem&&);
		~IMGUISystem();

		virtual void OnInitialize() override;

		forge::CallbackToken AddOverlayListener( const forge::Callback<>::TFunc& func )
		{
			return m_overlayCallback.AddListener( func );
		}

		imgui::TopBar& GetTopBar()
		{
			return m_topBar;
		}

	private:
		void DrawOverlay();

		std::unique_ptr< forge::IMGUIInstance > m_imguiInstance;

		forge::CallbackToken m_preUpdateToken;
		forge::CallbackToken m_updateToken;
		forge::CallbackToken m_postRenderingToken;

		forge::Callback<> m_overlayCallback;

		imgui::TopBar m_topBar;

		Bool m_imguiDemoEnabled = false;
	};
}

#endif