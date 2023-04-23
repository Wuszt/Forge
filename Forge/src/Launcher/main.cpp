#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../GameEngine/PublicDefaults.h"
#include "../Systems/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/RenderingSystem.h"
#include "../Renderer/Renderer.h"
#include "../Core/IInput.h"
#include "../Systems/LightingSystem.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/Material.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Core/Time.h"
#include "../Systems/TimeSystem.h"
#include "../Renderer/TextureAsset.h"
#include "../Core/AssetsManager.h"
#include "../Renderer/FBXLoader.h"
#include "../../External/imgui/imgui.h"
#include "../Renderer/AnimationSetAsset.h"
#include "../Renderer/SkeletonAsset.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

#ifdef FORGE_IMGUI_ENABLED
#include "../Systems/IMGUISystem.h"
#endif
#include "../GameEngine/ISystem.h"
#include "../Core/ArraySpan.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/AnimationComponent.h"
#include "../Systems/TransformSystem.h"
#include "../Physics/PhysxProxy.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/PhysicsComponent.h"
#include "../Physics/PhysicsShape.h"

void MinecraftScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\vokselia\\vokselia_spawn.obj" );

			transformComponent->GetDirtyData().m_transform.SetPosition( Vector3::ZEROS() );
			transformComponent->GetDirtyData().m_scale = { 1000.0f, 1000.0f, 1000.0f };
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetPosition( { 0.0f, -1100.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 1.0f, 0.0f, 0.0f };
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetPosition( { 0.0f, 0.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 0.0f, 1.0f, 0.0f };
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetPosition( { 0.0f, 1100.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 0.0f, 0.0f, 1.0f };
		} );
	} );
}

void SkeletalMesh( forge::EngineInstance& engineInstance, const Vector3& pos )
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ &engineInstance, pos ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent, forge::AnimationComponent >( [ &engineInstance, obj, pos ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();
			auto* animComponent = obj->GetComponent< forge::AnimationComponent >();

			std::string animName = "Animations\\Thriller.fbx";

			auto animation = engineInstance.GetAssetsManager().GetAsset< renderer::AnimationSetAsset >( animName );
			auto skeleton = engineInstance.GetAssetsManager().GetAsset< renderer::SkeletonAsset >( animName );

			renderingComponent->LoadMeshAndMaterial( animName );

			animComponent->SetSkeleton( *skeleton );
			animComponent->SetAnimation( animation->GetAnimations()[ 0 ] );

			transformComponent->GetDirtyData().m_transform = pos;
		} );
	} );
}

void SponzaScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\sponza\\sponza.obj" );

			transformComponent->GetDirtyData().m_transform.SetPosition( Vector3::ZEROS() );
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetPosition( { 0.0f, -1100.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 1.0f, 0.0f, 0.0f };
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::SpotLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetPosition( { 0.0f, 0.0f, 500.0f } );
			light->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetOrientation( Quaternion( -FORGE_PI_HALF, 0.0f, 0.0f ) );
			light->GetComponent< forge::SpotLightComponent >()->GetData().m_color = { 0.5f, 0.5f, 0.5f };
			light->GetComponent< forge::SpotLightComponent >()->GetData().m_power = 8000.0f;
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetPosition( { 0.0f, 1100.0f, 200.0f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 0.0f, 0.0f, 1.0f };
		} );
	} );
}

void BunnyScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\bunny.obj" );

			transformComponent->GetDirtyData().m_transform.SetPosition( Vector3::ZEROS() );
			transformComponent->GetDirtyData().m_transform.SetOrientation( Quaternion( 0.0f, 0.0f, FORGE_PI * 0.6f ) );
			transformComponent->GetDirtyData().m_scale = { 100.0f, 100.0f, 100.0f };
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );

			auto& renderable = renderingComponent->GetData().m_renderable;
			for( auto& material : renderable.GetMaterials() )
			{
				material->SetTexture( engineInstancePtr->GetAssetsManager().GetAsset< renderer::TextureAsset >( "Textures\\grass.jpg" )->GetTexture(), renderer::Material::TextureType::Diffuse );
			}

			transformComponent->GetDirtyData().m_transform.SetPosition( Vector3::ZEROS() );
			transformComponent->GetDirtyData().m_scale = { 1000.0f, 1000.0f, 0.01f };
		} );
	} );
}

void CubeScene( forge::EngineInstance& engineInstance )
{
	for( Uint32 i = 0u; i < 4000; ++i )
	{
		engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
			{
				obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsDynamicComponent >( [ engineInstancePtr = &engineInstance, obj ]()
					{
						auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
						transformComponent->GetDirtyData().m_transform.SetPosition( { Math::Random::GetRNG().GetFloat(-100.0f, 100.0f), Math::Random::GetRNG().GetFloat( -100.0f, 100.0f ), 250.0f + Math::Random::GetRNG().GetFloat( 0.0f, 25.0f ) });
						transformComponent->GetDirtyData().m_transform.SetOrientation( Quaternion::CreateFromDirection( Vector3{ Math::Random::GetRNG().GetFloat( -1.0f, 1.0f ), Math::Random::GetRNG().GetFloat( -1.0f, 1.0f ), Math::Random::GetRNG().GetFloat( -1.0f, 1.0f )}.Normalized() ) );
						transformComponent->GetDirtyData().m_scale = Vector3::ONES() * Math::Random::GetRNG().GetFloat( 1.0f, 5.0f );

						auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

						renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );
						renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
						renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();

						auto* physicsComponent = obj->GetComponent< forge::PhysicsDynamicComponent >();
						physicsComponent->GetActor().UpdateDensity( 1.0f );
						physicsComponent->AddShape( physics::PhysicsShape( engineInstancePtr->GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), Vector3( transformComponent->GetData().m_scale * 0.5f ) ) );
					} );
			} );
	}

	//for(Uint32 z = 0u; z < 5; ++z )
	//{
	//	for ( Uint32 x = 0u; x < 5; ++x )
	//	{
	//		for( Uint32 y = 0u; y < 5; ++y )
	//		{
	//			engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ &, x, y, z ]( forge::Object* obj )
	//				{
	//					obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsDynamicComponent >( [ engineInstancePtr = &engineInstance, obj, x, y, z ]()
	//						{
	//							const Float scale = 1.0f;

	//							auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
	//							transformComponent->GetDirtyData().m_transform.SetPosition( { static_cast< Float >( x ) * scale, static_cast< Float >( y ) * scale, static_cast< Float >( z ) + 1.0f } );
	//							transformComponent->GetDirtyData().m_scale = Vector3::ONES() * scale;

	//							auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

	//							renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );
	//							renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
	//							renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();

	//							auto* physicsComponent = obj->GetComponent< forge::PhysicsDynamicComponent >();
	//							physicsComponent->GetActor().UpdateDensity( 1.0f );
	//							physicsComponent->AddShape( physics::PhysicsShape( engineInstancePtr->GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), Vector3( transformComponent->GetData().m_scale * 0.5f ) ) );
	//						} );
	//				} );
	//		}
	//	}
	//}

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
		{
			light->RequestAddingComponents< forge::TransformComponent, forge::SpotLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
				{
					light->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetPosition( { 0.0f, 0.0f, 500.0f } );
					light->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetOrientation( Quaternion::CreateFromDirection( { 0.0f, 0.0f, -1.0f } ) );
					light->GetComponent< forge::SpotLightComponent >()->GetData().m_innerAngle = FORGE_PI_HALF * 0.5f;
					light->GetComponent< forge::SpotLightComponent >()->GetData().m_outerAngle = FORGE_PI_HALF * 0.75f;
				} );
		} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
		{
			obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsDynamicComponent >( [ engineInstancePtr = &engineInstance, obj ]()
				{
					auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
					transformComponent->GetDirtyData().m_transform.SetPosition( { 0.0f, 0.0f, 50.0f } );
					transformComponent->GetDirtyData().m_transform.SetOrientation( Quaternion::CreateFromDirection( Vector3{ Math::Random::GetRNG().GetFloat( -1.0f, 1.0f ), Math::Random::GetRNG().GetFloat( -1.0f, 1.0f ), Math::Random::GetRNG().GetFloat( -1.0f, 1.0f ) }.Normalized() ) );
					transformComponent->GetDirtyData().m_scale = Vector3::ONES() * 5.0f;

					auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

					renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );
					renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
					renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();

					auto* physicsComponent = obj->GetComponent< forge::PhysicsDynamicComponent >();
					physicsComponent->GetActor().UpdateDensity( 1.0f );
					physicsComponent->AddShape( physics::PhysicsShape( engineInstancePtr->GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), Vector3( transformComponent->GetData().m_scale * 0.5f ) ) );
				} );
		} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsStaticComponent  >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			transformComponent->GetDirtyData().m_transform.SetPosition( Vector3::ZEROS() );
			transformComponent->GetDirtyData().m_scale = { 1000.0f, 1000.0f, 0.01f };

			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );
			auto& renderable = renderingComponent->GetData().m_renderable;
			for ( auto& material : renderable.GetMaterials() )
			{
				material->SetTexture( engineInstancePtr->GetAssetsManager().GetAsset< renderer::TextureAsset >( "Textures\\grass.jpg" )->GetTexture(), renderer::Material::TextureType::Diffuse );
			}

			auto* physicsComponent = obj->GetComponent< forge::PhysicsStaticComponent >();
			physicsComponent->AddShape( physics::PhysicsShape( engineInstancePtr->GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), Vector3( transformComponent->GetData().m_scale * 0.5f ) ) );
		} );
	} );
}

void SphereScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\sphere.obj" );

			transformComponent->GetDirtyData().m_transform.SetPosition( Vector3::ZEROS() );

			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
		} );
	} );
}

Int32 main()
{
	class GameInstance : public forge::ApplicationInstance
	{
	public:
		using forge::ApplicationInstance::ApplicationInstance;

		virtual void Initialize( forge::EngineInstance& engineInstance )
		{
			const systems::ISystem::ClassType* systems[]
			{
				&systems::CamerasSystem::GetTypeStatic(),
				&systems::PlayerSystem::GetTypeStatic(),
				&systems::RenderingSystem::GetTypeStatic(),
				&systems::LightingSystem::GetTypeStatic(),
				&systems::TimeSystem::GetTypeStatic(),
				&systems::AnimationSystem::GetTypeStatic(),
				&systems::TransformSystem::GetTypeStatic(),
				&systems::PhysicsSystem::GetTypeStatic(),
#ifdef FORGE_DEBUGGING
				&systems::DebugSystem::GetTypeStatic(),
#endif

#ifdef FORGE_IMGUI_ENABLED
				&systems::IMGUISystem::GetTypeStatic()
#endif
			};
			engineInstance.GetSystemsManager().AddSystems( systems );

			engineInstance.GetSystemsManager().GetSystem< systems::LightingSystem >().SetAmbientColor( { 0.55f, 0.55f, 0.55f } );

			engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* player )
			{
				player->RequestAddingComponents< forge::TransformComponent, forge::CameraComponent, forge::FreeCameraControllerComponent >( [ engineInstancePtr = &engineInstance, player ]()
				{
					player->GetComponent< forge::TransformComponent >()->GetDirtyData().m_transform.SetPosition( { 0.0f, -400.0f, 200.0f } );
					auto* cameraComp = player->GetComponent< forge::CameraComponent >();
					cameraComp->CreateImplementation< renderer::PerspectiveCamera >( forge::CameraComponent::GetDefaultPerspectiveCamera( engineInstancePtr->GetWindow() ));

					auto& camerasSystem = engineInstancePtr->GetSystemsManager().GetSystem< systems::CamerasSystem >();
					camerasSystem.SetActiveCamera( cameraComp );

					auto* freeCameraController = player->GetComponent< forge::FreeCameraControllerComponent >();
					engineInstancePtr->GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController );
				} );
			} );

			//engineInstance.GetSystemsManager().GetSystem< systems::RenderingSystem >().SetSkyboxTexture( engineInstance.GetAssetsManager().GetAsset< renderer::TextureAsset >( "Textures\\skymap.dds" )->GetTexture() );

			//MinecraftScene( engineInstance );
			//SponzaScene( engineInstance );
			//BunnyScene( engineInstance );

			CubeScene(engineInstance);
			//SkeletalMesh( engineInstance, { 0.0f, 400.0f, 0.0f } );
			//SkeletalMesh( engineInstance, { 0.0f, 600.0f, 0.0f } );
			//SkeletalMesh( engineInstance, { 0.0f, 800.0f, 0.0f } );
		}

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{		
			if( engineInstance.GetWindow().GetInput()->GetKeyDown( forge::IInput::Key::Escape ) )
			{
				Shutdown();
			}
		}

		virtual Bool WithRendering() const override
		{
			return true;
		}

	private:
		forge::DirectionalLightComponent* m_sun = nullptr;

	} gameInstance( "Launcher" );

	forge::EngineInstance engineInstance( gameInstance );

	engineInstance.Run();
}