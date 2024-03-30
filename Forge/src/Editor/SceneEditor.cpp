#include "Fpch.h"
#include "SceneEditor.h"
#include "../../External/imgui/imgui.h"
#include "../Systems/SceneRenderingSystem.h"
#include "../GameEngine/RenderingManager.h"
#include "../IMGUI/IMGUIHelpers.h"
#include "../Renderer/IShaderResourceView.h"
#include "Gizmo.h"
#include "SceneGrid.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/CamerasSystem.h"
#include "../Systems/CameraComponent.h"
#include "../Physics/RaycastResult.h"
#include "../Systems/PhysicsUserData.h"
#include "../Core/IWindow.h"
#include "../Systems/InputSystem.h"

editor::SceneEditor::SceneEditor( forge::EngineInstance& engineInstance )
	: PanelBase( engineInstance )
{
	m_targetTexture = GetEngineInstance().GetRenderingManager().GetRenderer().CreateTexture( 512u, 512u,
		renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE, renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );

	systems::SceneRenderingSystem& sceneRenderingSystem = GetEngineInstance().GetSystemsManager().GetSystem<systems::SceneRenderingSystem>();
	sceneRenderingSystem.SetTargetTexture( m_targetTexture.get() );

	m_sceneGrid = std::make_unique< editor::SceneGrid >( engineInstance );
}

editor::SceneEditor::~SceneEditor() = default;

void editor::SceneEditor::Draw()
{
	if ( m_targetTexture->GetSize() != GetSize() )
	{
		if ( GetSize().X != 0.0f && GetSize().Y != 0.0f )
		{
			m_targetTexture->Resize( GetSize() );
		}
	}

	const Vector2 imageDrawPos = forge::imgui::CastToForge( ImGui::GetCursorScreenPos() );
	ImGui::Image( m_targetTexture->GetShaderResourceView()->GetRawSRV(), forge::imgui::CastToImGui( m_targetTexture->GetSize() ) );

	const Bool shouldProcessInput = ImGui::IsWindowFocused() && ImGui::IsItemHovered();
	GetEngineInstance().GetSystemsManager().GetSystem< systems::InputSystem >().Enable( false );

	if ( !shouldProcessInput )
	{
		return;
	}

	GetEngineInstance().GetSystemsManager().GetSystem< systems::InputSystem >().Enable( ImGui::IsMouseDown( ImGuiMouseButton_Right ) );

	forge::ObjectID objectId;
	Vector3 rayDir;
	const Vector2 cursorPos = forge::imgui::CastToForge( ImGui::GetMousePos() ) - imageDrawPos;
	Bool clickedThisFrame = GetEngineInstance().GetRenderingManager().GetWindow().GetInput().GetMouseButtonState( forge::IInput::MouseButton::LeftButton ) == forge::IInput::KeyState::Pressed;

	if( m_gizmoToken.GetObject() )
	{
		const Bool hoveringOverSomething = FindHoveredObject( cursorPos, physics::PhysicsGroupFlags::Editor, objectId, rayDir );

		editor::Gizmo& gizmo = *m_gizmoToken.GetObject< editor::Gizmo >();
		gizmo.Update( objectId, rayDir );

		if ( clickedThisFrame && !hoveringOverSomething )
		{
			m_gizmoToken = forge::ObjectLifetimeToken();
		}
	}

	if ( clickedThisFrame && m_gizmoToken.GetObject() == nullptr )
	{
		if ( FindHoveredObject( cursorPos, ~physics::PhysicsGroupFlags::Editor, objectId, rayDir ) )
		{
			GetEngineInstance().GetObjectsManager().RequestCreatingObject< editor::Gizmo >( [ this, objectId ]( editor::Gizmo* gizmo )
			{
				m_gizmoToken = forge::ObjectLifetimeToken( *gizmo );
				gizmo->Initialize( objectId );
			} );
		}
	}
}

bool editor::SceneEditor::FindHoveredObject( const Vector2& cursorPos, physics::PhysicsGroupFlags group, forge::ObjectID& outObjectId, Vector3& outRayDir ) const
{
	auto& camerasSystem = GetEngineInstance().GetSystemsManager().GetSystem<systems::CamerasSystem>();

	const auto& currentCamera = camerasSystem.GetActiveCamera()->GetCamera();
	const Vector3 cameraPos = currentCamera.GetPosition();

	const Vector2 resolution = GetSize();
	const Vector2 viewSpacePos = { 2.0f * ( cursorPos.X / resolution.X - 0.5f ), -2.0f * ( cursorPos.Y / resolution.Y - 0.5f ) };

	const Vector4 rayClip = { viewSpacePos.X, viewSpacePos.Y, 1.0f, 1.0f };
	outRayDir = currentCamera.GetInvViewMatrix().TransformVector( currentCamera.GetInvProjectionMatrix().TransformVector( rayClip ) ).Normalized3();

	auto& physicsSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::PhysicsSystem >();
	physics::RaycastResult result;
	if ( physicsSystem.PerformRaycast( cameraPos, outRayDir, std::numeric_limits< Float >::max(), result, group ) )
	{
		auto userData = physics::UserData( result.m_actorData );
		outObjectId = userData.m_objectId;

		return true;
	}

	return false;
}
