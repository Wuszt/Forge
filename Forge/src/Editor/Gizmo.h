#pragma once
#include "../GameEngine/Object.h"
#include "../GameEngine/ObjectLifetimeToken.h"

namespace editor
{
	class GizmoArrow : public forge::Object
	{
		RTTI_DECLARE_CLASS( GizmoArrow, forge::Object )

	public:
		using forge::Object::Object;
		virtual void OnAttach() override;
		void SetColor( const Vector4& color );
		void SetPosition( const Vector3& pos );

		void OnSelected( const Vector3& cursorRayDir );
		Vector3 GetDesiredPosition( const Vector3& cursorRayDir ) const;

	private:
		GizmoArrow() = default;
		GizmoArrow( GizmoArrow&& ) = default;

		Vector3 CastRayDirOnArrowAxis( const Vector3& cursorRayDir ) const;

		std::function<void( const Vector3& )> m_onMoved;

		Vector3 m_movementOffset;
	};

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

		forge::ObjectLifetimeToken m_xAxisArrow;
		forge::ObjectLifetimeToken m_yAxisArrow;
		forge::ObjectLifetimeToken m_zAxisArrow;

		forge::ObjectID m_modifiedObject;

		GizmoArrow* m_activeArrow = nullptr;
	};
}

