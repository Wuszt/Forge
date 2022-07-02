#pragma once

#ifdef FORGE_IMGUI_ENABLED
#include "ISystem.h"

namespace forge
{
	class IMGUIInstance;
}

namespace imgui
{
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

			FORGE_INLINE const char* GetName() const
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
			FORGE_INLINE std::vector< std::weak_ptr< Element > >& GetChildren()
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
				m_onClicked.AddListener( [ this ]()
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
		Bool m_selected = false;
	};

	using TopBarItemHandle = std::shared_ptr< TopBarItem >;
}

namespace systems
{
	class IMGUISystem : public ISystem
	{
		DECLARE_TYPE( IMGUISystem, systems, ISystem );
	public:
		IMGUISystem();
		~IMGUISystem();

		virtual void OnInitialize() override;

		FORGE_INLINE forge::CallbackToken AddOverlayListener( const forge::Callback<>::TFunc& func )
		{
			return m_overlayCallback.AddListener( func );
		}

		FORGE_INLINE imgui::TopBar& GetTopBar()
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