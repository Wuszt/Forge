#pragma once
#include "../GameEngine/Object.h"

namespace editor
{
	class GizmoElement;

	class Gizmo : public forge::Object
	{
	public:
		Gizmo( forge::EngineInstance& engineInstance, forge::ObjectID id );
		virtual void OnAttach() override;

		void Update( forge::ObjectID hoveredObject, const Vector3& cursorRayDir );
		void Initialize( forge::ObjectID modifiedObject );

	private:
		Gizmo() = default;
		Gizmo( Gizmo&& ) = default;

		std::vector< forge::ObjectLifetimeToken > m_elements;

		forge::ObjectID m_modifiedObject;

		GizmoElement* m_activeElement = nullptr;
	};
}

