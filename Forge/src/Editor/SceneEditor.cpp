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
#include "../IMGUI/IMGUIMenuBar.h"
#include "../Core/DepotsContainer.h"
#include "HierarchyView.h"

editor::SceneEditor::SceneEditor( forge::EngineInstance& engineInstance )
	: PanelBase( true, engineInstance )
{
	m_targetTexture = GetEngineInstance().GetRenderingManager().GetRenderer().CreateTexture( 512u, 512u,
		renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE, renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );

	systems::SceneRenderingSystem& sceneRenderingSystem = GetEngineInstance().GetSystemsManager().GetSystem<systems::SceneRenderingSystem>();
	sceneRenderingSystem.SetTargetTexture( m_targetTexture.get() );

	m_sceneGrid = std::make_unique< editor::SceneGrid >( engineInstance );
	m_hierarchyView = std::make_unique< HierarchyView >( false, engineInstance, [ this ]( forge::ObjectID id ){ SelectObject( id ); }, [ this ]() { return GetSelectedObject(); } );

	rtti::Get().VisitTypes( [ & ]( const rtti::Type& type )
		{
			if ( type.IsA< forge::Object >() || type.InheritsFrom< forge::Object >() )
			{
				const char editorNamespace[] = "editor::";
				constexpr Uint32 editorNamespaceLength = sizeof( editorNamespace ) - 1;

				if ( std::strncmp( type.GetName(), editorNamespace, editorNamespaceLength ) != 0 )
				{
					auto buttonHandle = GetMenuBar()->AddButton( { "Create Object", type.GetName() }, false );
					auto onCreationHandle = buttonHandle->GetCallback().AddListener( [ & ]()
						{
							const forge::Object::Type& asObjectType = static_cast< const forge::Object::Type& >( type );
							GetEngineInstance().GetObjectsManager().RequestCreatingObject( asObjectType );
						});
					m_objectCreationHandles.push_back( { buttonHandle, std::move( onCreationHandle ) } );
				}
			}
		} );
}

editor::SceneEditor::~SceneEditor() = default;

void editor::SceneEditor::SelectObject( forge::ObjectID objectID )
{
	m_selectedObjectID = objectID;
	m_gizmoToken.Reset();
	if ( m_selectedObjectID.IsValid() )
	{
		if ( GetEngineInstance().GetObjectsManager().GetObject( m_selectedObjectID )->GetComponent< forge::TransformComponent >() )
		{
			GetEngineInstance().GetObjectsManager().RequestCreatingObject< editor::Gizmo >( [ this ]( editor::Gizmo* gizmo )
				{
					m_gizmoToken = forge::ObjectLifetimeToken( *gizmo );
					gizmo->Initialize( m_selectedObjectID );
				} );
		}
	}
}

void editor::SceneEditor::Draw()
{
	m_hierarchyView->Update();

	if ( m_targetTexture->GetSize() != GetSize() )
	{
		if ( GetSize().X != 0.0f && GetSize().Y != 0.0f )
		{
			m_targetTexture->Resize( GetSize() );
		}
	}

	if ( ImGui::IsWindowHovered() && ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
	{
		ImGui::SetWindowFocus();
	}

	const Vector2 cursorPos = forge::imgui::CastToForge( ImGui::GetMousePos() ) - forge::imgui::CastToForge( ImGui::GetCursorScreenPos() );
	ImGui::Image( m_targetTexture->GetShaderResourceView()->GetRawSRV(), forge::imgui::CastToImGui( m_targetTexture->GetSize() ) );

	auto& inputSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::InputSystem >();
	inputSystem.Enable( ImGui::IsMouseDown( ImGuiMouseButton_Right ) );

	const Bool shouldProcessInput = ImGui::IsWindowFocused() && ImGui::IsItemHovered();
	if ( shouldProcessInput )
	{
		UpdateSelectedObject( cursorPos );
		UpdateGizmo( cursorPos );
	}
}

void editor::SceneEditor::UpdateSelectedObject( const Vector2& cursorPos )
{
	forge::ObjectID objectId;
	const Bool clickedThisFrame = GetEngineInstance().GetRenderingManager().GetWindow().GetInput().GetMouseButtonState( forge::IInput::MouseButton::LeftButton ) == forge::IInput::KeyState::Pressed;
	if ( clickedThisFrame )
	{
		const Bool hoveringOverEditorObject = FindHoveredObject( cursorPos, physics::PhysicsGroupFlags::Editor, objectId );
		if ( !hoveringOverEditorObject )
		{
			const Bool hoveringOverSceneObject = FindHoveredObject( cursorPos, ~physics::PhysicsGroupFlags::Editor, objectId );

			if ( hoveringOverSceneObject )
			{
				SelectObject( objectId );
			}
			else
			{
				SelectObject( forge::ObjectID() );
			}
		}
	}
}

void editor::SceneEditor::UpdateGizmo( const Vector2& cursorPos )
{
	if ( m_gizmoToken.IsValid() )
	{
		forge::ObjectID hoveredObjectId;
		FindHoveredObject( cursorPos, physics::PhysicsGroupFlags::Editor, hoveredObjectId );
		m_gizmoToken.GetObject< editor::Gizmo >()->Update( hoveredObjectId, GetMouseRayDir( cursorPos ) );
	}
}

bool editor::SceneEditor::FindHoveredObject( const Vector2& cursorPos, physics::PhysicsGroupFlags group, forge::ObjectID& outObjectId ) const
{
	auto& camerasSystem = GetEngineInstance().GetSystemsManager().GetSystem<systems::CamerasSystem>();

	const auto& currentCamera = camerasSystem.GetActiveCamera()->GetCamera();
	const Vector3 cameraPos = currentCamera.GetPosition();

	auto& physicsSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::PhysicsSystem >();
	physics::RaycastResult result;

	const Vector4 rayDir = GetMouseRayDir( cursorPos );

	if ( physicsSystem.PerformRaycast( cameraPos, rayDir, std::numeric_limits< Float >::max(), result, group ) )
	{
		auto userData = physics::UserData( result.m_actorData );
		outObjectId = userData.m_objectId;

		return true;
	}

	return false;
}

Vector3 editor::SceneEditor::GetMouseRayDir( const Vector2& cursorPos ) const
{
	auto& camerasSystem = GetEngineInstance().GetSystemsManager().GetSystem<systems::CamerasSystem>();
	const auto& currentCamera = camerasSystem.GetActiveCamera()->GetCamera();
	const Vector2 resolution = GetSize();
	const Vector2 viewSpacePos = { 2.0f * ( cursorPos.X / resolution.X - 0.5f ), -2.0f * ( cursorPos.Y / resolution.Y - 0.5f ) };
	const Vector4 rayClip = { viewSpacePos.X, viewSpacePos.Y, 1.0f, 1.0f };
	return currentCamera.GetInvViewMatrix().TransformVector( currentCamera.GetInvProjectionMatrix().TransformVector( rayClip ) ).Normalized3();
}
