#pragma once
#include "WindowBase.h"

namespace physics
{
	enum class PhysicsGroupFlags : Uint32;
}

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
	class SceneEditor;

	class SceneViewport : public WindowBase
	{
	public:
		SceneViewport( SceneEditor& sceneEditor );

	private:
		virtual void Draw() override;
		void UpdateSelectedObject( const Vector2& cursorPos );
		void UpdateGizmo( const Vector2& cursorPos );
		bool FindHoveredObject( const Vector2& cursorPos, physics::PhysicsGroupFlags group, forge::ObjectID& outObjectId ) const;
		Vector3 GetMouseRayDir( const Vector2& cursorPos ) const;

		SceneEditor& GetSceneEditor();
		const Char* GetName() const override
		{
			return "Scene Viewport";
		}

		std::unique_ptr< renderer::ITexture > m_targetTexture;
		std::unique_ptr< editor::SceneGrid > m_sceneGrid;
		forge::ObjectLifetimeToken m_gizmoToken;
		forge::CallbackToken m_onNewSelectedObjectToken;
	};
}

