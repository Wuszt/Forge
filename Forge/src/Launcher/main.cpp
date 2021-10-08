//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../Systems/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Core/FPSCounter.h"

#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/RenderingSystem.h"
#include "../Systems/IMGUISystem.h"
#include "../Systems/DebugSystem.h"

Int32 main()
{
	class GameInstance : public forge::ApplicationInstance
	{
	public:
		virtual void Initialize( forge::EngineInstance& engineInstance )
		{
			systems::SystemsManager::BootContext ctx;
			ctx.AddSystem< systems::CamerasSystem >();
			ctx.AddSystem< systems::PlayerSystem >();
			ctx.AddSystem< systems::RenderingSystem >();
			ctx.AddSystem< systems::IMGUISystem >();
			ctx.AddSystem< systems::DebugSystem >();

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

			Math::Random rng;

			const Uint32 dim = 200u;
			for( Uint32 i = 0; i < dim * dim; ++i )
			{
				if( rng.GetFloat() < 0.1f )
				{
					continue;
				}

				forge::Entity* entity = engineInstance.GetEntitiesManager().CreateEntity();
				entity->AddComponents< forge::TransformComponent, forge::RenderingComponent >();
				auto* transformComponent = entity->GetComponent< forge::TransformComponent >();
				auto* renderingComponent = entity->GetComponent< forge::RenderingComponent >();

				renderingComponent->GetRenderable()->GetConstantBuffer()->AddData( "m_color", Vector4( rng.GetFloat(), rng.GetFloat(), rng.GetFloat(), 1.0f ) );
				renderingComponent->GetRenderable()->GetConstantBuffer()->UpdateBuffer();

				renderingComponent->GetRenderable()->SetMesh( std::make_unique< renderer::CubeMesh >( engineInstance.GetRenderer() ) );
				renderingComponent->GetRenderable()->SetMaterial( std::make_unique< renderer::Material >( engineInstance.GetRenderer(), "Effects.fx", "Effects.fx" ) );

				Matrix m;

				Float scaleZ = ( rng.GetFloat() < 0.005f ? 2.0f : 1.0f ) * rng.GetFloat( 20.0f, 100.0f );
				Float scaleXY = rng.GetFloat( 10.0f, 20.0f );
				transformComponent->GetData().m_scale = { scaleXY, scaleXY, scaleZ };

				Float x = static_cast<Float>( i % dim ) * 50.0f;
				Float y = static_cast<Float>( i / dim ) * 50.0f;
				transformComponent->GetData().m_transform.SetPosition( { x - dim * 25, y - dim * 25, scaleZ } );
			}

			{
				forge::Entity* ground = engineInstance.GetEntitiesManager().CreateEntity();
				ground->AddComponents< forge::TransformComponent, forge::RenderingComponent >();
				auto* transformComponent = ground->GetComponent< forge::TransformComponent >();
				auto* renderingComponent = ground->GetComponent< forge::RenderingComponent >();

				renderingComponent->GetRenderable()->GetConstantBuffer()->AddData( "m_color", Vector4( 0.0f, 0.6f, 0.0f, 1.0f ) );
				renderingComponent->GetRenderable()->GetConstantBuffer()->UpdateBuffer();

				renderingComponent->GetRenderable()->SetMesh( std::make_unique< renderer::CubeMesh >( engineInstance.GetRenderer() ) );
				renderingComponent->GetRenderable()->SetMaterial( std::make_unique< renderer::Material >( engineInstance.GetRenderer(), "Effects.fx", "Effects.fx" ) );

				transformComponent->GetData().m_scale = { 20000.0f, 20000.0f, 0.01f };
				transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
			}
		}

		virtual Bool WithRendering() const override
		{
			return true;
		}
	} gameInstance;

	forge::EngineInstance engineInstance( gameInstance );

	engineInstance.Run();
}