#pragma once
#include "WindowBase.h"

namespace editor
{
	class SceneEditor;

	class SceneObjectView : public WindowBase
	{
	public:
		SceneObjectView( SceneEditor& sceneEditor );

		SceneEditor& GetSceneEditor();
	protected:
		void Draw() override;
		const Char* GetName() const override
		{
			return "Object View";
		}
	};
}

