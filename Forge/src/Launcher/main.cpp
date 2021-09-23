//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../Systems/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Core/FPSCounter.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../IMGUI/IMGUIInstance.h"
#include "../IMGUI/PublicDefaults.h"
#endif
#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/RenderingSystem.h"
#include "../Systems/IMGUISystem.h"

Int32 main()
{
	forge::EngineInstance engineInstance;

	systems::SystemsManager::BootContext ctx;
	ctx.AddSystem< systems::CamerasSystem >();
	ctx.AddSystem< systems::PlayerSystem >();
	ctx.AddSystem< systems::RenderingSystem >();
	ctx.AddSystem< systems::IMGUISystem >();

	engineInstance.GetSystemsManager().Boot( ctx );

	auto* player = engineInstance.GetEntitiesManager().CreateEntity();

	auto* transformComp = player->AddComponent< forge::TransformComponent >();
	transformComp->GetData().m_transform.SetPosition( { 0.0f, 0.0f, 50.0f } );

	auto* cameraComp = player->AddComponent< forge::CameraComponent >();
	cameraComp->CreateImplementation< forge::PerspectiveCamera >( engineInstance.GetWindow().GetAspectRatio(), FORGE_PI / 3.0f, 0.1f, 40000.0f );
	auto& camerasSystem = engineInstance.GetSystemsManager().GetSystem< systems::CamerasSystem >();
	camerasSystem.SetActiveCamera( cameraComp );

	auto* freeCameraController = player->AddComponent< forge::FreeCameraControllerComponent >();
	engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController );

	auto fpsOverlayDrawingToken = engineInstance.GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( []()
	{
		Float fps = forge::FPSCounter::GetAverageFPS( 1u );
		Vector4 color{ 1.0f, 0.0f, 0.0f, 1.0f };

		if( fps > 30.0f )
		{
			if( fps > 6.0f )
			{
				color = { 0.0f, 1.0f, 0.0f, 1.0f };
			}
			else
			{
				color = { 1.0f, 1.0f, 0.0f, 1.0f };
			}
		}

		Uint32 bufferSize = 0u;
		Uint32 currentOffset = 0u;
		Float* buff = forge::FPSCounter::GetFPSCounter().GetBuffer( currentOffset, bufferSize );

		ImGui::PlotHistogram( "", buff, bufferSize, currentOffset, NULL, 0.0f, 120.0f, ImVec2( 100.0f, 40.0f ) );
		ImGui::SameLine();

		ImGui::SetWindowFontScale( 1.5f );
		ImGui::PushStyleColor( ImGuiCol_Text, { color.X, color.Y, color.Z, color.W } );
		ImGui::Text( "FPS: %.2f", fps );
		ImGui::PopStyleColor();
		ImGui::SetWindowFontScale( 1.0f );
	} );

	Bool imguiExample = false;

	Math::Random rng;

	const Uint32 dim = 500u;
	for( Uint32 i = 0; i < dim * dim; ++i )
	{
		if( rng.GetFloat() > 0.9f )
		{
			continue;
		}

		forge::Entity* entity = engineInstance.GetEntitiesManager().CreateEntity();
		auto* transformComponent = entity->AddComponent< forge::DataComponent< forge::TransformComponentData > >();
		auto* renderingComponent = entity->AddComponent< forge::DataComponent< forge::RenderingComponentData > >();
		renderingComponent->GetData().m_color = Vector4( rng.GetFloat(), rng.GetFloat(), rng.GetFloat(), 1.0f );

		Matrix m;

		Float scaleZ = rng.GetFloat( 20.0f, 100.0f );
		Float scaleXY = rng.GetFloat( 10.0f, 20.0f );
		transformComponent->GetData().m_scale = { scaleXY, scaleXY, scaleZ };

		Float x = static_cast<Float>( i % dim ) * 50.0f;
		Float y = static_cast<Float>( i / dim ) * 50.0f;
		transformComponent->GetData().m_transform.SetPosition( { x - dim * 25, y - dim * 25, scaleZ } );
	}

	{
		forge::Entity* ground = engineInstance.GetEntitiesManager().CreateEntity();
		auto* transformComponent = ground->AddComponent< forge::DataComponent< forge::TransformComponentData > >();
		auto* renderingComponent = ground->AddComponent< forge::DataComponent< forge::RenderingComponentData > >();

		renderingComponent->GetData().m_color = Vector4( 0.0f, 0.6f, 0.0f, 1.0f );
		transformComponent->GetData().m_scale = { 20000.0f, 20000.0f, 0.01f };
		transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
	}

	engineInstance.Run();

	//ImGui::ShowDemoWindow( &imguiExample );
}