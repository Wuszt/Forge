
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
#include "../Renderer/IRenderer.h"
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
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );

			transformComponent->GetDirtyData().m_transform.SetPosition( Vector3::ZEROS() );

			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 1.0f, 1.0f, 1.0f } );
			renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
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

			//MinecraftScene( engineInstance );
			SponzaScene( engineInstance );
			//BunnyScene( engineInstance );

			//for ( Uint32 i = 0u; i < 15u; ++i )
			//{
			//	for ( Uint32 j = 0u; j < 15u; ++j )
			//	{
			//		SkeletalMesh( engineInstance, { -400.0f + j * 100.0f, -400.0f + i * 200.0f, 0.0f } );
			//	}
			//}

			SkeletalMesh( engineInstance, { 0.0f, 400.0f, 0.0f } );

			engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
			{
				light->RequestAddingComponents< forge::DirectionalLightComponent >( [ this, engineInstancePtr = &engineInstance, light ]()
				{
					m_sun = light->GetComponent< forge::DirectionalLightComponent >();
					light->GetComponent< forge::DirectionalLightComponent >()->GetData().Direction = { 0.0f, 0.0f, -1.0f };
				} );
			} );
		}

		Int32 frame = 0u;
		Uint32 deltaFrame = 1u;

		Float accumulator = 0.0f;
		Float speed = 1.0f;

		struct cbBones
		{
			Matrix Bones[ 70u ];
		};

		std::unique_ptr< renderer::StaticConstantBuffer< cbBones > > bonesBuffer;

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{		
			accumulator += engineInstance.GetSystemsManager().GetSystem< systems::TimeSystem >().GetDeltaTime() * speed;

			if( engineInstance.GetWindow().GetInput()->GetKeyDown( forge::IInput::Key::Escape ) )
			{
				Shutdown();
			}

			if( m_sun )
			{
				//Float currentAngle = DEG2RAD * 10.0f * engineInstance.GetSystemsManager().GetSystem< systems::TimeSystem >().GetCurrentTime();
				//m_sun->GetData().Direction = Quaternion( -FORGE_PI_HALF, 0.0f, currentAngle ) * Vector3::EY();
				m_sun->GetData().Color = Vector3{ 1.0f, 1.0f, 1.0f } * 0.3f;
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