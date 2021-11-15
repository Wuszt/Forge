//Include and link appropriate libraries and headers//

#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../Systems/PublicDefaults.h"

#include "../Core/IWindow.h"

#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/RenderingSystem.h"

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

#ifdef FORGE_DEBUGGING
			ctx.AddSystem< systems::DebugSystem >();
#endif

#ifdef FORGE_IMGUI_ENABLED
			ctx.AddSystem< systems::IMGUISystem >();
#endif

			engineInstance.GetSystemsManager().Boot( ctx );

			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* player )
			{
				player->RequestAddingComponents< forge::TransformComponent, forge::CameraComponent, forge::FreeCameraControllerComponent >( [ engineInstancePtr = &engineInstance, player ]()
				{
					player->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetPosition( { 0.0f, -400.0f, 200.0f } );
					auto* cameraComp = player->GetComponent< forge::CameraComponent >();
					cameraComp->CreateImplementation< forge::PerspectiveCamera >( engineInstancePtr->GetWindow().GetAspectRatio(), FORGE_PI / 3.0f, 0.1f, 40000.0f );
					auto& camerasSystem = engineInstancePtr->GetSystemsManager().GetSystem< systems::CamerasSystem >();
					camerasSystem.SetActiveCamera( cameraComp );

					auto* freeCameraController = player->GetComponent< forge::FreeCameraControllerComponent >();
					engineInstancePtr->GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController );
				} );
			} );

			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* car )
			{
				m_car = car;
				car->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, car ]()
				{
					auto* transformComponent = car->GetComponent< forge::TransformComponent >();
					auto* renderingComponent = car->GetComponent< forge::RenderingComponent >();

					renderingComponent->LoadMeshAndMaterial( "bmw.obj" );

					transformComponent->GetData().m_scale = { 0.1f, 0.1f, 0.1f };
				} );
			} );

			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* ground )
			{
				ground->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, ground ]()
				{
					auto* transformComponent = ground->GetComponent< forge::TransformComponent >();
					auto* renderingComponent = ground->GetComponent< forge::RenderingComponent >();

					renderingComponent->LoadMeshAndMaterial( "cube.obj" );

					transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
					transformComponent->GetData().m_scale = { 1000.0f, 1000.0f, 0.01f };
				} );
			} );

			for( Uint32 i = 0; i < 10000; ++i )
			{
				engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* car )
				{
					car->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ &, engineInstancePtr = &engineInstance, car ]()
					{
						auto* transformComponent = car->GetComponent< forge::TransformComponent >();
						auto* renderingComponent = car->GetComponent< forge::RenderingComponent >();

						renderingComponent->LoadMeshAndMaterial( "cube.obj" );

						transformComponent->GetData().m_transform.SetPosition( { m_rng.GetFloat( -1000.0f, 1000.0f ), m_rng.GetFloat( -1000.0f, 1000.0f ), 0.0f } );
						renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->SetData( "diffuseColor", Vector4( m_rng.GetFloat( 0.0f, 1.0f ), m_rng.GetFloat( 0.0f, 1.0f ), m_rng.GetFloat( 0.0f, 1.0f ), 1.0f ) );
						renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->UpdateBuffer();
					} );
				} );
			}
		}

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{
			m_timeBuffer += forge::Time::GetDeltaTime();
			if( m_timeBuffer > 1.0f )
			{
				return;
				for( Uint32 i = 0; i < 1000; ++i )
				{
					engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* car )
					{
						car->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ &, engineInstancePtr = &engineInstance, car ]()
						{
							auto* transformComponent = car->GetComponent< forge::TransformComponent >();
							auto* renderingComponent = car->GetComponent< forge::RenderingComponent >();

							renderingComponent->LoadMeshAndMaterial( "cube.obj" );

							transformComponent->GetData().m_transform.SetPosition( { m_rng.GetFloat( -1000.0f, 1000.0f ), m_rng.GetFloat( -1000.0f, 1000.0f ), 0.0f } );
							renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->SetData( "diffuseColor", Vector4( m_rng.GetFloat( 0.0f, 1.0f ), m_rng.GetFloat( 0.0f, 1.0f ), m_rng.GetFloat( 0.0f, 1.0f ), 1.0f ) );
							renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->UpdateBuffer();
						} );
					} );
				}
				m_timeBuffer = 0.0f;
			}
		}

		virtual Bool WithRendering() const override
		{
			return true;
		}

	private:
		Float m_timeBuffer = 0.0f;

		forge::Entity* m_car = nullptr;

		Math::Random m_rng;

	} gameInstance;

	forge::EngineInstance engineInstance( gameInstance );

	engineInstance.Run();
}