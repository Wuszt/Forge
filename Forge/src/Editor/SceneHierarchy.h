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
		void DrawObjectEntry( forge::ObjectID objectID );
		const Char* GetName() const override { return "Hierarchy"; }
	};
}

