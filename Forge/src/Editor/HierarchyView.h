#pragma once
#include "WindowBase.h"

namespace forge
{
	class ObjectID;
}

namespace editor
{
	class SceneEditor;
	class HierarchyView : public WindowBase
	{
	public:
		using SelectObjectFunc = std::function< void( forge::ObjectID ) >;
		using GetSelectedFunc = std::function< forge::ObjectID() >;

		HierarchyView( editor::SceneEditor& sceneEditor );

	protected:
		SceneEditor& GetSceneEditor();

		void Draw() override;
		const Char* GetName() const override { return "Hierarchy"; }
	};
}

