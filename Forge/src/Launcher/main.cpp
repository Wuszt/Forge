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
#include "../Renderer/IRenderer.h"
#include "../Core/IInput.h"
#include "../Systems/LightingSystem.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Core/Time.h"

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
				material->SetShaders( "Uber.fx", "Uber.fx", renderer::RenderingPass::Opaque );
			}

			transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
			transformComponent->GetData().m_scale = { 1000.0f, 1000.0f, 1000.0f };
		} );
	} );

	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetPosition( { 0.0f, -1100.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 1.0f, 0.0f, 0.0f };
		} );
	} );

	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetPosition( { 0.0f, 0.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 0.0f, 1.0f, 0.0f };
		} );
	} );

	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetPosition( { 0.0f, 1100.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 0.0f, 0.0f, 1.0f };
		} );
	} );
}

void SponzaScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "sponza.obj" );

			auto renderable = renderingComponent->GetData().m_renderable;
			for( auto& material : renderable->GetMaterials() )
			{
				material->SetShaders( "Uber.fx", "Uber.fx", renderer::RenderingPass::Opaque );
			}

			transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
		} );
	} );

	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetPosition( { 0.0f, -1100.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 1.0f, 0.0f, 0.0f };
		} );
	} );

	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::SpotLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetPosition( { 0.0f, 0.0f, 1000.0f } );
			light->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetOrientation( Quaternion( -FORGE_PI_HALF, 0.0f, 0.0f ) );
			light->GetComponent< forge::SpotLightComponent >()->GetData().m_color = { 0.62f, 0.74f, 0.3f };
			light->GetComponent< forge::SpotLightComponent >()->GetData().m_power = 8000.0f;
		} );
	} );

	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetPosition( { 0.0f, 1100.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 0.0f, 0.0f, 1.0f };
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
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
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
				material->SetShaders( "Uber.fx", "Uber.fx", renderer::RenderingPass::Opaque );
				material->SetTexture( "grass.jpg", renderer::Material::TextureType::Diffuse );
			}

			transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );
			transformComponent->GetData().m_scale = { 1000.0f, 1000.0f, 0.01f };
		} );
	} );
}

void CubeScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "cube.obj" );

			transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );

			renderingComponent->GetRenderable()->GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
		} );
	} );
}

void SphereScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "sphere.obj" );

			transformComponent->GetData().m_transform.SetPosition( Vector3::ZEROS() );

			renderingComponent->GetRenderable()->GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
			renderingComponent->GetRenderable()->GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
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
			ctx.AddSystem< systems::LightingSystem >();

#ifdef FORGE_DEBUGGING
			ctx.AddSystem< systems::DebugSystem >();
#endif

#ifdef FORGE_IMGUI_ENABLED
			ctx.AddSystem< systems::IMGUISystem >();
#endif

			engineInstance.GetSystemsManager().Boot( ctx );

			engineInstance.GetSystemsManager().GetSystem< systems::LightingSystem >().SetAmbientColor( { 0.05f, 0.05f, 0.05f } );

			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* player )
			{
				player->RequestAddingComponents< forge::TransformComponent, forge::CameraComponent, forge::FreeCameraControllerComponent >( [ engineInstancePtr = &engineInstance, player ]()
				{
					player->GetComponent< forge::TransformComponent >()->GetData().m_transform.SetPosition( { 0.0f, -400.0f, 200.0f } );
					auto* cameraComp = player->GetComponent< forge::CameraComponent >();
					cameraComp->CreateImplementation< renderer::PerspectiveCamera >( forge::CameraComponent::GetDefaultPerspectiveCamera( engineInstancePtr->GetWindow() ));

					auto& camerasSystem = engineInstancePtr->GetSystemsManager().GetSystem< systems::CamerasSystem >();
					camerasSystem.SetActiveCamera( cameraComp );

					auto* freeCameraController = player->GetComponent< forge::FreeCameraControllerComponent >();
					engineInstancePtr->GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController );
				} );
			} );

			//MinecraftScene( engineInstance );
			SponzaScene( engineInstance );
			//BunnyScene( engineInstance );	

			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* light )
			{
				light->RequestAddingComponents< forge::DirectionalLightComponent >( [ this, engineInstancePtr = &engineInstance, light ]()
				{
					m_sun = light->GetComponent< forge::DirectionalLightComponent >();
					light->GetComponent< forge::DirectionalLightComponent >()->GetData().Direction = { 0.0f, 0.0f, -1.0f };
				} );
			} );
		}

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{
			if( engineInstance.GetWindow().GetInput()->GetKeyDown( forge::IInput::Key::Escape ) )
			{
				Shutdown();
			}

			if( m_sun )
			{
				Float currentAngle = DEG2RAD * 10.0f * forge::Time::GetRealTime();
				m_sun->GetData().Direction = Quaternion( -FORGE_PI_HALF, 0.0f, currentAngle ) * Vector3::EY();
				m_sun->GetData().Color = Vector3{ 1.0f, 1.0f, 1.0f } *Math::Cos( currentAngle );
			}
		}

		virtual Bool WithRendering() const override
		{
			return true;
		}

	private:
		forge::DirectionalLightComponent* m_sun = nullptr;

	} gameInstance;

	forge::EngineInstance engineInstance( gameInstance );

	engineInstance.Run();
}