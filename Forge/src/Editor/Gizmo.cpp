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

namespace editor
{
	class GizmoElement : public forge::Object
	{
	public:
		using forge::Object::Object;
		virtual void OnAttach() override;
		virtual void OnSelected( const Vector3& cursorRayDir, const Vector3& currentScale ) {}
		void SetColor( const Vector4& color );
		virtual std::pair< Transform, Vector3 > GetDesiredTransformAndScale( const Vector3 & cursorRayDir, const Transform& currentTransform, const Vector3& currentScale ) const = 0;

	protected:
		virtual const char* GetModelPath() const = 0;
		Vector3 GetRayIntersectionWithPlane( const Vector3& cursorRayDir, const Vector3& planeNormal ) const;
	};

	class GizmoTranslationArrow : public GizmoElement
	{
	public:
		using GizmoElement::GizmoElement;
		virtual void OnSelected( const Vector3& cursorRayDir, const Vector3& currentScale ) override;
		virtual std::pair< Transform, Vector3 > GetDesiredTransformAndScale( const Vector3& cursorRayDir, const Transform& currentTransform, const Vector3& currentScale ) const override;

	protected:
		virtual const char* GetModelPath() const override
		{
			return "Models/Gizmo/gizmoArrow.fbx";
		}

	private:
		Float m_movementOffset = 0.0f;
	};

	class GizmoOrientationRing : public GizmoElement
	{
	public:
		using GizmoElement::GizmoElement;
		virtual void OnSelected( const Vector3& cursorRayDir, const Vector3& currentScale ) override;
		virtual std::pair< Transform, Vector3 > GetDesiredTransformAndScale( const Vector3& cursorRayDir, const Transform& currentTransform, const Vector3& currentScale ) const override;

	protected:
		virtual const char* GetModelPath() const override
		{
			return "Models/Gizmo/gizmoRing.fbx";
		}

	private:
		Transform m_initialTransform;
		Quaternion m_rotationOffset;
	};

	class GizmoUniformScaleCube : public GizmoElement
	{
	public:
		using GizmoElement::GizmoElement;
		virtual void PostInit() override;
		virtual void OnSelected( const Vector3& cursorRayDir, const Vector3& currentScale ) override;
		virtual std::pair< Transform, Vector3 > GetDesiredTransformAndScale( const Vector3& cursorRayDir, const Transform& currentTransform, const Vector3& currentScale ) const override;

	protected:
		virtual const char* GetModelPath() const override
		{
			return "Models/Cube.obj";
		}

	private:
		Vector3 m_initialScale;
		Vector3 m_initialCursorPos;
	};

	class GizmoAxisScaleCube : public GizmoElement
	{
	public:
		using GizmoElement::GizmoElement;

		virtual void PostInit() override;
		virtual void OnSelected( const Vector3& cursorRayDir, const Vector3& currentScale ) override;
		virtual std::pair< Transform, Vector3 > GetDesiredTransformAndScale( const Vector3& cursorRayDir, const Transform& currentTransform, const Vector3& currentScale ) const override;

	protected:
		virtual const char* GetModelPath() const override
		{
			return "Models/Cube.obj";
		}

	private:
		Vector3 m_initialScale;
		Vector3 m_initialCursorPos;
	};
}

editor::Gizmo::Gizmo( forge::EngineInstance& engineInstance, forge::ObjectID id )
	: forge::Object( engineInstance, id )
{}

void editor::Gizmo::OnAttach()
{
	forge::Object::OnAttach();

	AddComponents< forge::TransformComponent >();

	auto CreateGizmoElement = [ this ]< class T >( const Vector3& direction, const Vector4& color )
	{
		GetEngineInstance().GetObjectsManager().RequestCreatingObject< T >( [ this, direction, color ]( T* element )
		{
			m_elements.emplace_back( std::move( forge::ObjectLifetimeToken( *element ) ) );
			element->SetColor( color );

			element->GetComponent< forge::TransformComponent >()->SetParent( *GetComponent< forge::TransformComponent >(), false );
			element->GetComponent< forge::TransformComponent >()->SetRelativeOrientation( Quaternion::CreateFromDirection( direction ) );
		} );
	};

	const Vector4 c_xAxisColor( 1.0f, 0.0f, 0.0f, 1.0f );
	const Vector4 c_yAxisColor( 0.0f, 1.0f, 0.0f, 1.0f );
	const Vector4 c_zAxisColor( 0.0f, 0.0f, 1.0f, 1.0f );

	CreateGizmoElement.template operator()< editor::GizmoTranslationArrow > ( Vector3::EX(), c_xAxisColor );
	CreateGizmoElement.template operator()< editor::GizmoTranslationArrow > ( Vector3::EY(), c_yAxisColor );
	CreateGizmoElement.template operator()< editor::GizmoTranslationArrow > ( Vector3::EZ(), c_zAxisColor );

	CreateGizmoElement.template operator() < editor::GizmoOrientationRing > ( Vector3::EX(), c_xAxisColor );
	CreateGizmoElement.template operator() < editor::GizmoOrientationRing > ( Vector3::EY(), c_yAxisColor );
	CreateGizmoElement.template operator() < editor::GizmoOrientationRing > ( Vector3::EZ(), c_zAxisColor );

	CreateGizmoElement.template operator() < editor::GizmoAxisScaleCube > ( Vector3::EX(), c_xAxisColor );
	CreateGizmoElement.template operator() < editor::GizmoAxisScaleCube > ( Vector3::EY(), c_yAxisColor );
	CreateGizmoElement.template operator() < editor::GizmoAxisScaleCube > ( Vector3::EZ(), c_zAxisColor );

	CreateGizmoElement.template operator() < editor::GizmoUniformScaleCube >( Vector3::EY(), Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
}

void editor::Gizmo::Update( forge::ObjectID hoveredObject, const Vector3& cursorRayDir )
{
	forge::TransformComponent* modifiedTransformComp = GetEngineInstance().GetObjectsManager().GetObject( m_modifiedObject )->GetComponent< forge::TransformComponent >();

	auto mouseState = GetEngineInstance().GetRenderingManager().GetWindow().GetInput()->GetMouseButtonState( forge::IInput::MouseButton::LeftButton );
	switch ( mouseState )
	{
	case forge::IInput::KeyState::Clicked:
	{
		Bool anyClicked = false;
		for ( const forge::ObjectLifetimeToken& element : m_elements )
		{
			if ( hoveredObject == element.GetObjectId() )
			{
				m_activeElement = element.GetObject< editor::GizmoElement >();
				anyClicked = true;
			}
		}

		if ( anyClicked )
		{
			m_activeElement->OnSelected( cursorRayDir, modifiedTransformComp->GetWorldScale() );
		}
		else
		{
			m_activeElement = nullptr;
		}
	}

		break;

	case forge::IInput::KeyState::Held:
		if ( m_activeElement )
		{
			auto [desiredTransform, desiredScale] = m_activeElement->GetDesiredTransformAndScale( cursorRayDir, modifiedTransformComp->GetWorldTransform(), modifiedTransformComp->GetWorldScale() );
			modifiedTransformComp->SetWorldTransform( desiredTransform );
			modifiedTransformComp->SetWorldScale( desiredScale );
		}
		break;

	case forge::IInput::KeyState::Released:
		m_activeElement = nullptr;
		break;
	}

	forge::TransformComponent* gizmoTransformComp = GetComponent< forge::TransformComponent >();
	gizmoTransformComp->SetWorldTransform( modifiedTransformComp->GetWorldTransform() );

	{
		const auto& currentCamera = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera();
		constexpr Float distToScaleFactor = 0.15f;
		const Vector3 cameraPos = currentCamera.GetPosition();
		const Vector3 cameraToGizmo = gizmoTransformComp->GetWorldPosition() - cameraPos;
		const Float distToCamera = cameraToGizmo.Dot( currentCamera.GetTransform().GetForward() );
		gizmoTransformComp->SetWorldScale( Vector3::ONES() * distToCamera * distToScaleFactor );
	}
}

void editor::Gizmo::Initialize( forge::ObjectID modifiedObject )
{
	m_modifiedObject = modifiedObject;

	const forge::TransformComponent* modifiedTransformComp = GetEngineInstance().GetObjectsManager().GetObject( m_modifiedObject )->GetComponent< forge::TransformComponent >();
	GetComponent< forge::TransformComponent >()->SetWorldTransform( modifiedTransformComp->GetWorldTransform() );
}

void editor::GizmoElement::OnAttach()
{
	forge::Object::OnAttach();

	AddComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsStaticComponent >();
	auto* renderingComponent = GetComponent< forge::RenderingComponent >();
	renderingComponent->LoadMeshAndMaterial( GetModelPath() );
	renderingComponent->SetDrawAsOverlayEnabled( true );

	auto* physicsComponent = GetComponent< forge::PhysicsStaticComponent >();
	physicsComponent->SetGroup( physics::PhysicsGroupFlags::Editor );

	{
		auto modelAsset = GetEngineInstance().GetAssetsManager().GetAsset< renderer::ModelAsset >( GetModelPath() );

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

void editor::GizmoElement::SetColor( const Vector4& color )
{
	renderer::ConstantBuffer* materialBuffer = GetComponent< forge::RenderingComponent >()->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer();
	materialBuffer->SetData( "diffuseColor", color );
	materialBuffer->UpdateBuffer();
}

void editor::GizmoTranslationArrow::OnSelected( const Vector3& cursorRayDir, const Vector3& currentScale )
{
	const forge::TransformComponent* transformComponent = GetComponent< forge::TransformComponent >();
	const Transform& arrowTransform = transformComponent->GetWorldTransform();
	const Transform& cameraTransform = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera().GetTransform();
	m_movementOffset = ( GetRayIntersectionWithPlane( cursorRayDir, -cameraTransform.GetForward() ) - arrowTransform.GetPosition() ).Dot( arrowTransform.GetForward() );
}

std::pair< Transform, Vector3 > editor::GizmoTranslationArrow::GetDesiredTransformAndScale( const Vector3& cursorRayDir, const Transform& currentTransform, const Vector3& currentScale ) const
{
	const forge::TransformComponent* arrowTransformComponent = GetComponent< forge::TransformComponent >();
	const Transform& arrowTransform = arrowTransformComponent->GetWorldTransform();
	const Transform& cameraTransform = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera().GetTransform();

	const Float distanceFromArrowOrigin = ( GetRayIntersectionWithPlane( cursorRayDir, -cameraTransform.GetForward() ) - arrowTransform.GetPosition() ).Dot( arrowTransform.GetForward() ) - m_movementOffset;
	const Vector3 rayIntersectionWithArrow = arrowTransform.GetPosition() + arrowTransform.GetForward() * distanceFromArrowOrigin;
	const Quaternion currentOrientation = arrowTransform.GetOrientation() * arrowTransformComponent->GetRelativeOrientation().Inverted();
	return { Transform( rayIntersectionWithArrow, currentOrientation ), currentScale };
}

Vector3 editor::GizmoElement::GetRayIntersectionWithPlane( const Vector3& cursorRayDir, const Vector3& planeNormal ) const
{
	const Transform& transform = GetComponent< forge::TransformComponent >()->GetWorldTransform();
	const Vector3& arrowPos = transform.GetPosition3();

	const Transform& cameraTransform = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera().GetTransform();
	const Vector3& cameraPos = cameraTransform.GetPosition3();

	const Vector4 p( planeNormal, arrowPos.Dot( planeNormal ) );
	const Vector3 r = cursorRayDir;
	const Vector3 l = cameraPos;

	const Float t = ( p.W - p.X * l.X - p.Y * l.Y - p.Z * l.Z ) / ( p.X * r.X + p.Y * r.Y + p.Z * r.Z );

	return cameraPos + cursorRayDir * t;
}

std::pair< Transform, Vector3 > editor::GizmoOrientationRing::GetDesiredTransformAndScale( const Vector3& cursorRayDir, const Transform& currentTransform, const Vector3& currentScale ) const
{
	const auto* transformComponent = GetComponent< forge::TransformComponent >();
	const Vector3 cursorOnPlane = ( GetRayIntersectionWithPlane( cursorRayDir, -m_initialTransform.GetForward() ) - m_initialTransform.GetPosition() ).Normalized();
	const Quaternion cursorRotation = Quaternion::CreateFromDirection( cursorOnPlane, m_initialTransform.GetRight(), m_initialTransform.GetForward() );
	const Quaternion desiredOrientation = cursorRotation * m_rotationOffset * m_initialTransform.GetOrientation() * transformComponent->GetRelativeOrientation().Inverted();

	return { { transformComponent->GetWorldPosition(), desiredOrientation }, currentScale };
}

void editor::GizmoOrientationRing::OnSelected( const Vector3& cursorRayDir , const Vector3& currentScale )
{
	m_initialTransform = GetComponent< forge::TransformComponent >()->GetWorldTransform();
	const Vector3 rayDirOnPlane = ( GetRayIntersectionWithPlane( cursorRayDir, -m_initialTransform.GetForward() ) - m_initialTransform.GetPosition() ).Normalized();
	m_rotationOffset = Quaternion::GetRotationBetweenVectors( rayDirOnPlane, m_initialTransform.GetRight(), m_initialTransform.GetForward() );
}

void editor::GizmoUniformScaleCube::PostInit()
{
	GetComponent< forge::TransformComponent >()->SetRelativeScale( Vector3( 0.15f ) );
}

void editor::GizmoUniformScaleCube::OnSelected( const Vector3& cursorRayDir, const Vector3& currentScale )
{
	m_initialScale = currentScale;
	const Transform& cameraTransform = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera().GetTransform();
	m_initialCursorPos = GetRayIntersectionWithPlane( cursorRayDir, -cameraTransform.GetForward() );
}

std::pair< Transform, Vector3 > editor::GizmoUniformScaleCube::GetDesiredTransformAndScale( const Vector3& cursorRayDir, const Transform& currentTransform, const Vector3& currentScale ) const
{
	auto* transformComp = GetComponent< forge::TransformComponent >();
	const Transform& cameraTransform = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera().GetTransform();
	const Vector3 currentCursorPos = GetRayIntersectionWithPlane( cursorRayDir, -cameraTransform.GetForward() );

	const Vector3 dir = transformComp->GetWorldOrientation() * Vector3::ONES();
	Vector3 desiredScale = m_initialScale + Vector3( currentCursorPos.Dot( dir ) - m_initialCursorPos.Dot( dir ) );
	desiredScale = { Math::Abs( desiredScale.X ), Math::Abs( desiredScale.Y ), Math::Abs( desiredScale.Z ) };

	return { currentTransform, desiredScale };
}


void editor::GizmoAxisScaleCube::PostInit()
{
	auto* transformComp = GetComponent< forge::TransformComponent >();
	transformComp->SetRelativePosition( transformComp->GetRelativeTransform().GetForward() * 0.25f );
	transformComp->SetRelativeScale( Vector3( 1.0f, 3.0f, 1.0f ) * 0.075f );
}

void editor::GizmoAxisScaleCube::OnSelected( const Vector3& cursorRayDir, const Vector3& currentScale )
{
	m_initialScale = currentScale;
	const Transform& cameraTransform = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera().GetTransform();
	m_initialCursorPos = GetRayIntersectionWithPlane( cursorRayDir, -cameraTransform.GetForward() );
}

std::pair< Transform, Vector3 > editor::GizmoAxisScaleCube::GetDesiredTransformAndScale( const Vector3& cursorRayDir, const Transform& currentTransform, const Vector3& currentScale ) const
{
	auto* transformComp = GetComponent< forge::TransformComponent >();
	const Transform& cameraTransform = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera().GetTransform();
	const Vector3 currentCursorPos = GetRayIntersectionWithPlane( cursorRayDir, -cameraTransform.GetForward() );

	const Vector3 axisDir = transformComp->GetWorldTransform().GetForward();
	Vector3 desiredScale = m_initialScale + transformComp->GetRelativeTransform().GetForward() * ( currentCursorPos.Dot( axisDir ) - m_initialCursorPos.Dot( axisDir ) );
	desiredScale = { Math::Abs( desiredScale.X ), Math::Abs( desiredScale.Y ), Math::Abs( desiredScale.Z ) };

	return { currentTransform, desiredScale };
}
