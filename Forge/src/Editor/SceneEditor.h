#pragma once
#include "PanelBase.h"
#include "../GameEngine/ObjectLifetimeToken.h"

namespace forge
{
	class EngineInstance;
}

namespace renderer
{
	class ITexture;
}

namespace editor
{
	class SceneGrid;
	class Gizmo;

	class SceneEditor : public PanelBase
	{
	public:
		SceneEditor( forge::EngineInstance& engineInstance );
		~SceneEditor();

	protected:
		virtual void Draw() override;
		virtual const Char* GetName() const override
		{
			return "Scene Editor";
		}

	private:
		bool FindHoveredObject( const Vector2& cursorPos, forge::ObjectID& outObjectId, Vector3& outRayDir ) const;

		std::unique_ptr< renderer::ITexture > m_targetTexture;
		std::unique_ptr< editor::SceneGrid > m_sceneGrid;
		forge::ObjectLifetimeToken m_gizmoToken;
	};
}