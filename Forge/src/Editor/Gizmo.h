#pragma once
#include "../GameEngine/Object.h"

namespace editor
{
	class GizmoElement;

	class Gizmo : public forge::Object
	{
		RTTI_DECLARE_CLASS( Gizmo, forge::Object );

	public:
		virtual void OnInit( forge::ObjectInitData& initData ) override;

		void OnInput( forge::ObjectID hoveredObject, const Vector3& cursorRayDir );
		void Initialize( forge::ObjectID modifiedObject );

	private:
		Gizmo() = default;
		Gizmo( Gizmo&& ) = default;

		void Update();

		std::vector< forge::ObjectLifetimeToken > m_elements;

		forge::ObjectID m_modifiedObject;

		GizmoElement* m_activeElement = nullptr;

		forge::CallbackToken m_updateToken;
	};
}

