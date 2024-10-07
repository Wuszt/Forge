#include "Fpch.h"
#include "EditorInstance.h"
#include "../GameEngine/EngineInstance.h"
#include "../GameEngine/RenderingManager.h"
#include "../Core/IWindow.h"
#include "../Core/IInput.h"
#include "../GameEngine/ObjectsManager.h"
#include "../Systems/TransformComponent.h"
#include "../Systems/RenderingComponent.h"
#include "../Systems/PhysicsComponent.h"
#include "../GameEngine/Object.h"
#include "../GameEngine/ISystem.h"
#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/SceneRenderingSystem.h"
#include "../Systems/LightingSystem.h"
#include "../Systems/TimeSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../GameEngine/SystemsManager.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ISwapchain.h"
#include "../GameEngine/UpdateManager.h"
#include "../../External/imgui/imgui.h"
#include "SceneEditor.h"
#include "WindowBase.h"
#include "../Systems/InputSystem.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

#include "../Systems/IMGUISystem.h"
#include "../IMGUI/IMGUIHelpers.h"
#include "EditorTags.h"
#include "../GameEngine/SceneObject.h"

forge::EditorInstance::EditorInstance( const std::string& applicationName )
	: forge::ApplicationInstance( applicationName )
{}

forge::EditorInstance::~EditorInstance() = default;

void forge::EditorInstance::Initialize(forge::EngineInstance& engineInstance)
{
	m_engineInstance = &engineInstance;
	m_updateToken = engineInstance.GetUpdateManager().RegisterUpdateFunction(forge::UpdateManager::BucketType::Update, [this](){ Update(); });

	const systems::ISystem::Type* systems[]
	{
		&systems::CamerasSystem::GetTypeStatic(),
		&systems::PlayerSystem::GetTypeStatic(),
		&systems::SceneRenderingSystem::GetTypeStatic(),
		&systems::LightingSystem::GetTypeStatic(),
		&systems::TimeSystem::GetTypeStatic(),
		&systems::AnimationSystem::GetTypeStatic(),
		&systems::TransformSystem::GetTypeStatic(),
		&systems::PhysicsSystem::GetTypeStatic(),
		&systems::InputSystem::GetTypeStatic(),
#ifdef FORGE_DEBUGGING
		&systems::DebugSystem::GetTypeStatic(),
#endif

#ifdef FORGE_IMGUI_ENABLED
		&systems::IMGUISystem::GetTypeStatic()
#endif
	};

	engineInstance.GetSystemsManager().AddSystems( systems );
	engineInstance.GetSystemsManager().GetSystem< systems::LightingSystem >().SetAmbientColor({ 0.55f, 0.55f, 0.55f });

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >([ & ]( forge::Object* player )
	{
		const auto entityID = engineInstance.GetObjectsManager().GetOrCreateEntityId( player->GetObjectID() );
		player->SetName( "Player" );
		player->AddComponents< forge::TransformComponent, forge::CameraComponent, forge::PhysicsFreeCameraControllerComponent >();
		player->GetComponent< forge::TransformComponent >()->SetWorldPosition({ 0.0f, -5.0f, 0.0f });
		auto* cameraComp = player->GetComponent< forge::CameraComponent >();
		cameraComp->CreateImplementation< renderer::PerspectiveCamera >(forge::CameraComponent::GetDefaultPerspectiveCamera( engineInstance.GetRenderingManager().GetWindow() ) );

		auto& camerasSystem = engineInstance.GetSystemsManager().GetSystem< systems::CamerasSystem >();
		camerasSystem.SetActiveCamera(cameraComp);

		auto* freeCameraController = player->GetComponent< forge::PhysicsFreeCameraControllerComponent >();
		engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController);
	});

	m_windows.emplace_back( std::make_unique< editor::SceneEditor >( nullptr, engineInstance ) );
}

void forge::EditorInstance::Deinitialize( EngineInstance& engineInstance )
{
	m_updateToken.Unregister();
	m_windows.clear();
}

static void StyleColorsEditor()
{
	// Red Oni style by DatRobotix from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2( 8.0f, 8.0f );
	style.WindowRounding = 8.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2( 32.0f, 32.0f );
	style.WindowTitleAlign = ImVec2( 0.0f, 0.5f );
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 8.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2( 4.0f, 3.0f );
	style.FrameRounding = 6.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2( 8.0f, 4.0f );
	style.ItemInnerSpacing = ImVec2( 4.0f, 4.0f );
	style.CellPadding = ImVec2( 4.0f, 2.0f );
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 10.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_None;
	style.ButtonTextAlign = ImVec2( 0.5f, 0.5f );
	style.SelectableTextAlign = ImVec2( 0.0f, 0.0f );

	style.Colors[ ImGuiCol_Text ] = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
	style.Colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f );
	style.Colors[ ImGuiCol_WindowBg ] = ImVec4( 0.0f, 0.08235294371843338f, 0.1647058874368668f, 1.0f );
	style.Colors[ ImGuiCol_ChildBg ] = ImVec4( 0.0f, 0.0f, 0.0f, 0.0f );
	style.Colors[ ImGuiCol_PopupBg ] = ImVec4( 0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 1.0f );
	style.Colors[ ImGuiCol_Border ] = ImVec4( 1.0f, 0.0f, 0.4117647111415863f, 0.5f );
	style.Colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.0f, 0.0f, 0.0f, 0.0f );
	style.Colors[ ImGuiCol_FrameBg ] = ImVec4( 0.2196078449487686f, 0.2196078449487686f, 0.2196078449487686f, 0.5400000214576721f );
	style.Colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.6509804129600525f, 0.6509804129600525f, 0.6509804129600525f, 0.4000000059604645f );
	style.Colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.6700000166893005f );
	style.Colors[ ImGuiCol_TitleBg ] = ImVec4( 0.1019607856869698f, 0.1019607856869698f, 0.1019607856869698f, 1.0f );
	style.Colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.1019607856869698f, 0.1019607856869698f, 0.1019607856869698f, 1.0f );
	style.Colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.0f, 0.0f, 0.0f, 0.5099999904632568f );
	style.Colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.1333333402872086f, 0.1333333402872086f, 0.1333333402872086f, 1.0f );
	style.Colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.125490203499794f, 0.125490203499794f, 0.125490203499794f, 1.0f );
	style.Colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f );
	style.Colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 1.0f, 0.1372549086809158f, 0.4901960790157318f, 1.0f );
	style.Colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.8156862854957581f, 0.1215686276555061f, 0.407843142747879f, 1.0f );
	style.Colors[ ImGuiCol_CheckMark ] = ImVec4( 0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f );
	style.Colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f );
	style.Colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 1.0f, 0.0f, 0.5137255191802979f, 1.0f );
	style.Colors[ ImGuiCol_Button ] = ImVec4( 0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f );
	style.Colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.8588235378265381f, 0.2666666805744171f, 0.5098039507865906f, 1.0f );
	style.Colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f );
	style.Colors[ ImGuiCol_Header ] = ImVec4( 0.5803921818733215f, 0.0f, 0.239215686917305f, 1.0f );
	style.Colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f );
	style.Colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.95686274766922f, 0.0f, 0.3921568691730499f, 1.0f );
	style.Colors[ ImGuiCol_Separator ] = ImVec4( 0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f );
	style.Colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 0.7799999713897705f );
	style.Colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.2745098173618317f, 0.3411764800548553f, 0.4156862795352936f, 1.0f );
	style.Colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f );
	style.Colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.8039215803146362f, 0.364705890417099f, 0.5450980663299561f, 1.0f );
	style.Colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.9019607901573181f, 0.1098039224743843f, 0.4313725531101227f, 1.0f );
	style.Colors[ ImGuiCol_Tab ] = ImVec4( 0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.8619999885559082f );
	style.Colors[ ImGuiCol_TabHovered ] = ImVec4( 0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 0.800000011920929f );
	style.Colors[ ImGuiCol_TabActive ] = ImVec4( 0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f );
	style.Colors[ ImGuiCol_TabUnfocused ] = ImVec4( 0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f );
	style.Colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f );
	style.Colors[ ImGuiCol_PlotLines ] = ImVec4( 0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f );
	style.Colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f );
	style.Colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f );
	style.Colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.0f, 0.1176470592617989f, 0.47843137383461f, 1.0f );
	style.Colors[ ImGuiCol_TableHeaderBg ] = ImVec4( 0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f );
	style.Colors[ ImGuiCol_TableBorderStrong ] = ImVec4( 0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f );
	style.Colors[ ImGuiCol_TableBorderLight ] = ImVec4( 0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f );
	style.Colors[ ImGuiCol_TableRowBg ] = ImVec4( 0.0f, 0.0f, 0.0f, 0.0f );
	style.Colors[ ImGuiCol_TableRowBgAlt ] = ImVec4( 1.0f, 1.0f, 1.0f, 0.05999999865889549f );
	style.Colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f );
	style.Colors[ ImGuiCol_DragDropTarget ] = ImVec4( 1.0f, 1.0f, 0.0f, 0.8999999761581421f );
	style.Colors[ ImGuiCol_NavHighlight ] = ImVec4( 0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f );
	style.Colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.0f, 1.0f, 1.0f, 0.699999988079071f );
	style.Colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f );
	style.Colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f );
}

void forge::EditorInstance::Update()
{
	if ( m_engineInstance->GetRenderingManager().GetWindow().GetInput().GetKeyDown( forge::IInput::Key::Escape ) )
	{
		Shutdown();
	}

	forge::imgui::ScopedStyle style( StyleColorsEditor );
	ImGui::DockSpaceOverViewport( ImGui::GetMainViewport() );

	for ( auto& window : m_windows )
	{
		window->Update();
	}
}