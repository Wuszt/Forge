#pragma once
#include "WindowBase.h"

namespace forge
{
	class ObjectID;
}

namespace editor
{
	class SceneEditor;
	class SceneHierarchy : public WindowBase
	{
	public:
		SceneHierarchy( editor::SceneEditor& sceneEditor );

	protected:
		SceneEditor& GetSceneEditor();

		void Draw() override;
		void DrawObjectEntry( forge::ObjectID objectID, std::unordered_set< forge::ObjectID >& drawnObjects );
		const Char* GetName() const override { return "Hierarchy"; }
	};
}

