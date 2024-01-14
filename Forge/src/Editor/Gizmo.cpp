#include "Fpch.h"
#include "Gizmo.h"
#include "../Systems/TransformComponent.h"
#include "../Systems/RenderingComponent.h"
#include "../Renderer/Material.h"
#include "../Systems/PhysicsComponent.h"
#include "../Renderer/ModelAsset.h"
#include "../Renderer/IVertexBuffer.h"
#include "../Renderer/Model.h"
#include "../Physics/PhysicsShape.h"
#include "../Systems/PhysicsSystem.h"
#include "../Core/AssetsManager.h"
#include "../Core/IInput.h"
#include "../GameEngine/RenderingManager.h"
#include "../Core/IWindow.h"
#include "../Systems/CamerasSystem.h"
#include "../Systems/CameraComponent.h"

RTTI_IMPLEMENT_TYPE( editor::GizmoArrow )

const Vector4 c_xAxisColor( 1.0f, 0.0f, 0.0f, 1.0f );
const Vector4 c_yAxisColor( 0.0f, 1.0f, 0.0f, 1.0f );
const Vector4 c_zAxisColor( 0.0f, 0.0f, 1.0f, 1.0f );

void editor::Gizmo::OnAttach()
{
	forge::Object::OnAttach();

	AddComponents< forge::TransformComponent >();

	GetEngineInstance().GetObjectsManager().RequestCreatingObject< editor::GizmoArrow >( [ this ]( editor::GizmoArrow* arrow )
		{
			m_xAxisArrow = std::move( forge::ObjectLifetimeToken( *arrow ) );
			arrow->SetColor( c_xAxisColor );

			arrow->GetComponent< forge::TransformComponent >()->SetParent( *GetComponent< forge::TransformComponent >(), false );
			arrow->GetComponent< forge::TransformComponent >()->SetRelativeOrientation( Quaternion::CreateFromDirection( Vector3::EX() ) );
		} );

	GetEngineInstance().GetObjectsManager().RequestCreatingObject< editor::GizmoArrow >( [ this ]( editor::GizmoArrow* arrow )
		{
			m_yAxisArrow = std::move( forge::ObjectLifetimeToken( *arrow ) );
			arrow->SetColor( c_yAxisColor );

			arrow->GetComponent< forge::TransformComponent >()->SetParent( *GetComponent< forge::TransformComponent >(), false );
			arrow->GetComponent< forge::TransformComponent >()->SetRelativeOrientation( Quaternion::CreateFromDirection( Vector3::EY() ) );
		} );

	GetEngineInstance().GetObjectsManager().RequestCreatingObject< editor::GizmoArrow >( [ this ]( editor::GizmoArrow* arrow )
		{
			m_zAxisArrow = std::move( forge::ObjectLifetimeToken( *arrow ) );
			arrow->SetColor( c_zAxisColor );

			arrow->GetComponent< forge::TransformComponent >()->SetParent( *GetComponent< forge::TransformComponent >(), false );
			arrow->GetComponent< forge::TransformComponent >()->SetRelativeOrientation( Quaternion::CreateFromDirection( Vector3::EZ() ) );
		} );
}

void editor::Gizmo::Update( forge::ObjectID hoveredObject, const Vector3& cursorRayDir )
{
	forge::TransformComponent* gizmoTransformComp = GetComponent< forge::TransformComponent >();
	const auto& currentCamera = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera();
	const Vector3 cameraPos = currentCamera.GetPosition();
	const Vector3 cameraToGizmo = gizmoTransformComp->GetWorldPosition() - cameraPos;
	const Float distToCamera = cameraToGizmo.Dot( currentCamera.GetTransform().GetForward() );

	constexpr Float distToScaleFactor = 0.15f;
	gizmoTransformComp->SetWorldScale( Vector3::ONES() * distToCamera * distToScaleFactor );

	forge::TransformComponent* modifiedTransformComp = GetEngineInstance().GetObjectsManager().GetObject( m_modifiedObject )->GetComponent< forge::TransformComponent >();

	auto mouseState = GetEngineInstance().GetRenderingManager().GetWindow().GetInput()->GetMouseButtonState( forge::IInput::MouseButton::LeftButton );
	switch ( mouseState )
	{
	case forge::IInput::KeyState::Clicked:
		if ( hoveredObject == m_xAxisArrow.GetObjectId() )
		{
			m_activeArrow = m_xAxisArrow.GetObject< editor::GizmoArrow >();
		}
		else if ( hoveredObject == m_yAxisArrow.GetObjectId() )
		{
			m_activeArrow = m_yAxisArrow.GetObject< editor::GizmoArrow >();
		}
		else if ( hoveredObject == m_zAxisArrow.GetObjectId() )
		{
			m_activeArrow = m_zAxisArrow.GetObject< editor::GizmoArrow >();
		}
		else
		{
			m_activeArrow = nullptr;
		}

		if ( m_activeArrow )
		{
			m_activeArrow->OnSelected( cursorRayDir );
		}

		break;

	case forge::IInput::KeyState::Held:
		if ( m_activeArrow )
		{
			const Vector3 pos = m_activeArrow->GetDesiredPosition( cursorRayDir );
			modifiedTransformComp->SetWorldPosition( pos );
		}
		break;

	case forge::IInput::KeyState::Released:
		m_activeArrow = nullptr;
		break;
	}

	gizmoTransformComp->SetWorldPosition( modifiedTransformComp->GetWorldPosition() );
}

void editor::Gizmo::Initialize( forge::ObjectID modifiedObject )
{
	m_modifiedObject = modifiedObject;

	const forge::TransformComponent* modifiedTransformComp = GetEngineInstance().GetObjectsManager().GetObject( m_modifiedObject )->GetComponent< forge::TransformComponent >();
	GetComponent< forge::TransformComponent >()->SetWorldPosition( modifiedTransformComp->GetWorldPosition() );
}

editor::Gizmo::Gizmo( forge::EngineInstance& engineInstance, forge::ObjectID id )
	: forge::Object( engineInstance, id )
{}

void editor::GizmoArrow::OnAttach()
{
	Super::OnAttach();

	AddComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsStaticComponent >();
	auto* renderingComponent = GetComponent< forge::RenderingComponent >();
	renderingComponent->LoadMeshAndMaterial( "Models/arrow.fbx" );
	renderingComponent->SetDrawAsOverlayEnabled( true );

	auto* physicsComponent = GetComponent< forge::PhysicsStaticComponent >();
	physicsComponent->SetGroup( physics::PhysicsGroupFlags::Editor );

	{
		auto modelAsset = GetEngineInstance().GetAssetsManager().GetAsset< renderer::ModelAsset >( "Models/arrow.fbx" );

		auto model = modelAsset->GetModel();
		const renderer::Vertices& vertices = model->GetVertices();
		std::vector< Vector3 > verts;
		verts.resize( vertices.GetVerticesAmount() );

		FORGE_ASSERT( vertices.GetInputElements().begin()->m_inputType == renderer::InputType::Position );
		const Byte* address = static_cast< const Byte* >( vertices.GetData() );
		for ( Vector3& vec : verts )
		{
			vec = *reinterpret_cast< const Vector3* >( address );
			address += vertices.GetVertexSize();
		}

		Uint32 index = 0u;
		for ( renderer::Model::Shape& shape : model->GetShapes() )
		{
			physicsComponent->AddShape( physics::PhysicsShape( GetEngineInstance().GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), verts, shape.m_indices ) );
			++index;
		}
	}
}

void editor::GizmoArrow::SetColor( const Vector4& color )
{
	renderer::ConstantBuffer* materialBuffer = GetComponent< forge::RenderingComponent >()->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer();
	materialBuffer->SetData( "diffuseColor", color );
	materialBuffer->UpdateBuffer();
}

void editor::GizmoArrow::SetPosition( const Vector3& pos )
{
	GetComponent< forge::TransformComponent >()->SetWorldPosition( pos );
}

void editor::GizmoArrow::OnSelected( const Vector3& cursorRayDir )
{
	m_movementOffset = CastRayDirOnArrowAxis( cursorRayDir ) - GetComponent< forge::TransformComponent >()->GetWorldPosition();
}

Vector3 editor::GizmoArrow::GetDesiredPosition( const Vector3& cursorRayDir ) const
{
	return CastRayDirOnArrowAxis( cursorRayDir ) - m_movementOffset;
}

Vector3 editor::GizmoArrow::CastRayDirOnArrowAxis( const Vector3& cursorRayDir ) const
{
	const Transform& transform = GetComponent< forge::TransformComponent >()->GetWorldTransform();
	const Vector3& arrowPos = transform.GetPosition3();

	const Transform& cameraTransform = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera().GetTransform();
	const Vector3& cameraPos = cameraTransform.GetPosition3();
	const Vector3 cameraForward = cameraTransform.GetForward();

	const Vector4 p( -cameraForward, arrowPos.Dot( -cameraForward ) );
	const Vector3 r = cursorRayDir;
	const Vector3 l = cameraPos;

	const Float t = ( p.W - p.X * l.X - p.Y * l.Y - p.Z * l.Z ) / ( p.X * r.X + p.Y * r.Y + p.Z * r.Z );

	const Vector3 intersectionPoint = cameraPos + cursorRayDir * t;
	return transform.GetForward() * ( intersectionPoint ).Dot( transform.GetForward() );
}
