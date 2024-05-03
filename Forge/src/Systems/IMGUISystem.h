#pragma once
#include "../GameEngine/ISystem.h"

#ifdef FORGE_IMGUI_ENABLED

namespace forge
{
	class IMGUIInstance;
	class InputHandler;
}

namespace imgui
{
	class MenuBar;
}

namespace systems
{
	class IMGUISystem : public ISystem
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( IMGUISystem, systems::ISystem );
	public:
		IMGUISystem();
		IMGUISystem( IMGUISystem&& );
		~IMGUISystem();

		virtual void OnPostInit() override;
		virtual void OnDeinitialize() override;

		forge::CallbackToken AddOverlayListener( const forge::Callback<>::TFunc& func )
		{
			return m_overlayCallback.AddListener( func );
		}

		imgui::MenuBar& GetMenuBar()
		{
			return *m_menuBar;
		}

	private:
		void DrawOverlay();

		std::unique_ptr< forge::IMGUIInstance > m_imguiInstance;
		std::unique_ptr< forge::InputHandler > m_inputHandler;

		forge::CallbackToken m_preUpdateToken;
		forge::CallbackToken m_updateToken;
		forge::CallbackToken m_postRenderingToken;

		forge::Callback<> m_overlayCallback;

		std::unique_ptr< imgui::MenuBar > m_menuBar;

		Bool m_isInputActive = false;
	};
}

#endif