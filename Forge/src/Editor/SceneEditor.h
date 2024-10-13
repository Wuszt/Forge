#pragma once
#include "WindowBase.h"
#include "../GameEngine/ObjectID.h"

namespace forge
{
	class EngineInstance;
}

namespace imgui
{
	class MenuBar;
	class MenuBarItem;
}

namespace editor
{
	class SceneEditor : public WindowBase
	{
	public:
		SceneEditor( editor::WindowBase* parent, forge::EngineInstance& engineInstance );
		~SceneEditor();

		void SelectObject( forge::ObjectID objectID );
		forge::ObjectID GetSelectedObject() const
		{
			return m_selectedObjectID;
		}

		[[ nodiscard ]] forge::CallbackToken RegisterOnNewSelectedObject( forge::Callback< forge::ObjectID >::TFunc func )
		{
			return m_onNewSelectedObject.AddListener( std::move( func ) );
		}

	protected:
		virtual void Draw() override;
		virtual const Char* GetName() const override
		{
			return "Scene Editor";
		}

	private:
		forge::Callback< forge::ObjectID > m_onNewSelectedObject;
		forge::ObjectID m_selectedObjectID;
		std::vector< std::shared_ptr< imgui::MenuBarItem > > m_objectCreationButtons;
		std::shared_ptr< imgui::MenuBarItem > m_newSceneButton;
		std::shared_ptr< imgui::MenuBarItem > m_saveSceneButton;
		std::shared_ptr< imgui::MenuBarItem > m_saveAsSceneButton;
		std::shared_ptr< imgui::MenuBarItem > m_loadSceneButton;

		forge::CallbackToken m_onImportAssetClickedToken;
		forge::CallbackToken m_onNewSceneToken;
	};
}