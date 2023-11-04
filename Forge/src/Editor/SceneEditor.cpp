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

editor::SceneEditor::SceneEditor( forge::EngineInstance& engineInstance )
	: PanelBase( engineInstance )
{
	m_targetTexture = GetEngineInstance().GetRenderingManager().GetRenderer().CreateTexture( 512u, 512u,
		renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE, renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );

	systems::SceneRenderingSystem& sceneRenderingSystem = GetEngineInstance().GetSystemsManager().GetSystem<systems::SceneRenderingSystem>();
	sceneRenderingSystem.SetTargetTexture( m_targetTexture.get() );

	m_sceneGrid = std::make_unique< editor::SceneGrid >( engineInstance );
	engineInstance.GetObjectsManager().RequestCreatingObject< editor::Gizmo >( [ this ]( editor::Gizmo* gizmo )
		{
			m_gizmoToken = forge::ObjectLifetimeToken( *gizmo );
		} );
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

	if ( !ImGui::IsItemHovered( ImGuiHoveredFlags_None ) )
	{
		return;
	}

	forge::ObjectID objectId;
	Vector3 rayDir;
	FindHoveredObject( forge::imgui::CastToForge( ImGui::GetMousePos() ) - imageDrawPos, objectId, rayDir );

	editor::Gizmo& gizmo = *m_gizmoToken.GetObject< editor::Gizmo >();
	gizmo.Update( objectId, rayDir );
}

bool editor::SceneEditor::FindHoveredObject( const Vector2& cursorPos, forge::ObjectID& outObjectId, Vector3& outRayDir ) const
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
	if ( physicsSystem.PerformRaycast( cameraPos + outRayDir * 5.0f, outRayDir, std::numeric_limits< Float >::max(), result ) )
	{
		physics::UserData userData = physics::UserData::GetFromRaycastResult( result );
		outObjectId = userData.objectId;

		return true;
	}

	return false;
}
