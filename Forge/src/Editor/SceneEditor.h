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

namespace imgui
{
	class MenuBar;
	class MenuBarItem;
}

namespace editor
{
	class SceneGrid;
	class Gizmo;

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

	protected:
		virtual void Draw() override;
		virtual const Char* GetName() const override
		{
			return "Scene Editor";
		}

	private:
		void UpdateSelectedObject( const Vector2& cursorPos );
		void UpdateGizmo( const Vector2& cursorPos );
		bool FindHoveredObject( const Vector2& cursorPos, physics::PhysicsGroupFlags group, forge::ObjectID& outObjectId ) const;
		Vector3 GetMouseRayDir( const Vector2& cursorPos ) const;

		std::unique_ptr< renderer::ITexture > m_targetTexture;
		std::unique_ptr< editor::SceneGrid > m_sceneGrid;
		forge::ObjectLifetimeToken m_gizmoToken;
		forge::ObjectID m_selectedObjectID;
		
		struct ObjectCreationEntry
		{
			std::shared_ptr< imgui::MenuBarItem > m_createObjectButtonHandle;
			forge::CallbackToken m_onCreateObjectToken;
		};

		std::vector< ObjectCreationEntry > m_objectCreationHandles;
	};
}