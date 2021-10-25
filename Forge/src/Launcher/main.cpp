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

		Math::Random m_rng;

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

			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ &engineInstance ]( forge::Entity* player )
			{
				player->RequestAddingComponents< forge::TransformComponent, forge::CameraComponent, forge::FreeCameraControllerComponent >( [ engineInstancePtr = &engineInstance, player ]()
				{
					player->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetPosition( { 0.0f, 0.0f, 50.0f } );
					auto* cameraComp = player->GetComponent< forge::CameraComponent >();
					cameraComp->CreateImplementation< forge::PerspectiveCamera >( engineInstancePtr->GetWindow().GetAspectRatio(), FORGE_PI / 3.0f, 0.1f, 40000.0f );
					auto& camerasSystem = engineInstancePtr->GetSystemsManager().GetSystem< systems::CamerasSystem >();
					camerasSystem.SetActiveCamera( cameraComp );

					auto* freeCameraController = player->GetComponent< forge::FreeCameraControllerComponent >();
					engineInstancePtr->GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController );
				} );				
			} );

			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ &engineInstance ]( forge::Entity* ground )
			{
				ground->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, ground ]()
				{
					auto* transformComponent = ground->GetComponent< forge::TransformComponent >();
					auto* renderingComponent = ground->GetComponent< forge::RenderingComponent >();

					renderingComponent->GetRenderable()->SetMesh( std::make_unique< renderer::CubeMesh >( engineInstancePtr->GetRenderer() ) );
					renderingComponent->GetRenderable()->SetMaterial( std::make_unique< renderer::Material >( engineInstancePtr->GetRenderer(), "Effects.fx", "Effects.fx" ) );

					renderingComponent->GetRenderable()->GetMaterial()->GetConstantBuffer()->AddData( "m_color", Vector4( 0.0f, 0.6f, 0.0f, 1.0f ) );
					renderingComponent->GetRenderable()->GetMaterial()->GetConstantBuffer()->UpdateBuffer();

					transformComponent->GetData().m_scale = { 20000.0f, 20000.0f, 0.01f };
					transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
				} );
			} );
		}

		Float m_timeBuffer = 0.0f;

		std::vector< forge::EntityID > m_ids;

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{
			m_timeBuffer += forge::Time::GetDeltaTime();
			
			if( m_timeBuffer > 1.0f )
			{
				m_timeBuffer = 0.0f;
				const Float mapSize = 5000.0f;

				for( Uint32 i = 0; i < 1000; ++i )
				{
					CreateBuilding( engineInstance, { m_rng.GetFloat( -1.0f, 1.0f ) * mapSize, m_rng.GetFloat( -1.0f, 1.0f ) * mapSize }, m_rng.GetRaw() );
				}
			}
		}

		virtual Bool WithRendering() const override
		{
			return true;
		}

	private:
		void CreateBuilding( forge::EngineInstance& engineInstance, const Vector2& pos, Uint32 seed )
		{
			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ =, &engineInstance ]( forge::Entity* entity )
			{
				entity->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ pos, entity, engineInstancePtr = &engineInstance, seed ]()
				{
					Math::Random rng( seed );
					auto* transformComponent = entity->GetComponent< forge::TransformComponent >();
					auto* renderingComponent = entity->GetComponent< forge::RenderingComponent >();

					renderingComponent->GetRenderable()->SetMesh( std::make_unique< renderer::CubeMesh >( engineInstancePtr->GetRenderer() ) );
					renderingComponent->GetRenderable()->SetMaterial( std::make_unique< renderer::Material >( engineInstancePtr->GetRenderer(), "Effects.fx", "Effects.fx" ) );

					renderingComponent->GetRenderable()->GetMaterial()->GetConstantBuffer()->AddData( "m_color", Vector4( rng.GetFloat(), rng.GetFloat(), rng.GetFloat(), 1.0f ) );
					renderingComponent->GetRenderable()->GetMaterial()->GetConstantBuffer()->UpdateBuffer();

					Matrix m;

					Float scaleZ = rng.GetFloat( 20.0f, 100.0f );
					Float scaleXY = rng.GetFloat( 10.0f, 20.0f );
					transformComponent->GetData().m_scale = { scaleXY, scaleXY, scaleZ };
					transformComponent->GetData().m_transform.SetPosition( { pos.X, pos.Y, scaleZ } );
				} );
			} );
		}

	} gameInstance;

	forge::EngineInstance engineInstance( gameInstance );

	engineInstance.Run();
}