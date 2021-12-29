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
#include "../D3D11Renderer/D3D11TexturesLoader.h"
#include "../Renderer/IRenderer.h"

void MinecraftScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "vokselia_spawn.obj" );

			auto renderable = renderingComponent->GetData().m_renderable;
			for( auto& material : renderable->GetMaterials() )
			{
				material.SetVertexShader( "Texture.fx" );
				material.SetPixelShader( "Texture.fx" );
			}

			transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
			transformComponent->GetData().m_scale = { 1000.0f, 1000.0f, 1000.0f };
		} );
	} );
}

void BunnyScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "bunny.obj" );

			transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
			transformComponent->GetData().m_transform.SetOrientation( Quaternion( 0.0f, 0.0f, FORGE_PI * 0.6f ) );
			transformComponent->GetData().m_scale = { 100.0f, 100.0f, 100.0f };
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ].GetConstantBuffer()->UpdateBuffer();
		} );
	} );

	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "cube.obj" );

			auto renderable = renderingComponent->GetData().m_renderable;
			for( auto& material : renderable->GetMaterials() )
			{
				material.SetVertexShader( "Texture.fx" );
				material.SetPixelShader( "Texture.fx" );
				material.SetTexture( "grass.jpg", 0 );
			}

			transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
			transformComponent->GetData().m_scale = { 1000.0f, 1000.0f, 0.01f };
		} );
	} );
}

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

			engineInstance.GetSystemsManager().GetSystem< systems::RenderingSystem >().SetSamplers( { renderer::SamplerStateFilterType::MIN_MAG_MIP_LINEAR } );

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

			BunnyScene(engineInstance);
		}

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{
			//m_timeBuffer += forge::Time::GetDeltaTime();

			//Math::Random rng;
			//if( m_timeBuffer > 0.2f )
			//{
			//	if( ++m_qwe % 2u != 0u )
			//	{
			//		for( Uint32 i = 0; i < rng.GetUnsigned( 0u, 75u ); ++i )
			//		{
			//			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* car )
			//			{
			//				m_entities.emplace_back( car->GetEntityID() );

			//				car->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ &, engineInstancePtr = &engineInstance, car ]()
			//				{
			//					auto* transformComponent = car->GetComponent< forge::TransformComponent >();
			//					auto* renderingComponent = car->GetComponent< forge::RenderingComponent >();

			//					renderingComponent->LoadMeshAndMaterial( "bmw.obj" );

			//					transformComponent->GetData().m_transform.SetPosition( { m_rng.GetFloat( -1000.0f, 1000.0f ), m_rng.GetFloat( -1000.0f, 1000.0f ), 0.0f } );
			//					transformComponent->GetData().m_scale = { 0.1f, 0.1f, 0.1f };
			//				} );
			//			} );
			//		}
			//	}
			//	else
			//	{
			//		for( Uint32 i = 0; i < Math::Min( static_cast< Uint32 >( m_entities.size() ), rng.GetUnsigned( 0u, 100u ) ); ++i )
			//		{
			//			Uint32 index = rng.GetUnsigned( 0u, static_cast< Uint32 >( m_entities.size() ) - 1u );
			//			engineInstance.GetEntitiesManager().RequestDestructingEntity( m_entities[ index ] );
			//			forge::utils::RemoveReorder( m_entities, index );
			//		}
			//		
			//	}

			//	m_timeBuffer = 0.0f;
			//}
		}

		virtual Bool WithRendering() const override
		{
			return true;
		}

	private:
		Float m_timeBuffer = 0.0f;

		forge::Entity* m_car = nullptr;

		Math::Random m_rng;

		std::vector< forge::EntityID > m_entities;

		Uint32 m_qwe = 0u;

	} gameInstance;

	forge::EngineInstance engineInstance( gameInstance );

	engineInstance.Run();
}