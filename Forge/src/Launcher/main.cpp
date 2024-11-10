#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../GameEngine/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/SceneRenderingSystem.h"
#include "../Systems/LightingSystem.h"
#include "../Renderer/Material.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Systems/TimeSystem.h"
#include "../Renderer/TextureAsset.h"
#include "../Core/AssetsManager.h"
#include "../Renderer/SkeletonAsset.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/AnimationComponent.h"
#include "../Systems/TransformSystem.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/PhysicsComponent.h"
#include "../Physics/PhysicsShape.h"
#include "../Renderer/ModelAsset.h"
#include "../Renderer/Model.h"
#include "../Physics/RaycastResult.h"
#include "../GameEngine/RenderingManager.h"
#include "../Renderer/ISwapchain.h"
#include "../Systems/InputSystem.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

#ifdef FORGE_IMGUI_ENABLED
#endif

void SkeletalMesh( forge::EngineInstance& engineInstance, const Vector3& pos )
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ &engineInstance, pos ]( forge::Object& obj, forge::ObjectInitData& )
	{
		obj.AddComponents< forge::TransformComponent, forge::RenderingComponent, forge::AnimationComponent >();
		auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
		auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();
		auto* animComponent = obj.GetComponent< forge::AnimationComponent >();

		forge::Path animPath( "Animations\\Thriller.fbx" );

		auto animation = engineInstance.GetAssetsManager().GetAsset< renderer::AnimationSetAsset >( animPath );
		auto skeleton = engineInstance.GetAssetsManager().GetAsset< renderer::SkeletonAsset >( animPath );

		renderingComponent->LoadMeshAndMaterial( animPath );

		animComponent->SetSkeleton( *skeleton );
		animComponent->SetAnimation( animation->GetAnimations()[ 0 ] );

		transformComponent->SetWorldPosition( pos );
	} } );
}

void SponzaScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ & ]( forge::Object& obj, forge::ObjectInitData& )
	{
		obj.AddComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsStaticComponent >();

		auto* transformComponent = obj.GetComponent< forge::TransformComponent >();
		auto* renderingComponent = obj.GetComponent< forge::RenderingComponent >();

		renderingComponent->LoadMeshAndMaterial( forge::Path( "Launcher\\Models\\sponza\\sponza.obj" ) );

		transformComponent->SetWorldPosition( Vector3::ZEROS() );
		transformComponent->SetWorldScale( Vector3::ONES() * 0.01f );

		auto* physicsComponent = obj.GetComponent< forge::PhysicsStaticComponent >();
		auto modelAsset = engineInstance.GetAssetsManager().GetAsset< renderer::ModelAsset >( forge::Path( "Launcher\\Models\\sponza\\sponza.obj" ) );

		auto model = modelAsset->GetModel();
		const renderer::Vertices& vertices = model->GetVertices();
		std::vector< Vector3 > verts;
		verts.resize( vertices.GetVerticesAmount() );

		FORGE_ASSERT( vertices.GetInputElements().begin()->m_inputType == renderer::InputType::Position );
		const Byte* address = static_cast< const Byte* >( vertices.GetData() );
		for ( Vector3& vec : verts )
		{
			vec = *reinterpret_cast< const Vector3* >( address );
			address += vertices.GetVertexSize();
		}

		Uint32 index = 0u;
		for ( renderer::Model::Shape& shape : model->GetShapes() )
		{
			physicsComponent->AddShape( physics::PhysicsShape( engineInstance.GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), verts, shape.m_indices ) );
			++index;
		}
	} } );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ & ]( forge::Object& light, forge::ObjectInitData& )
	{
		light.AddComponents< forge::TransformComponent, forge::PointLightComponent >();
		light.GetComponent< forge::TransformComponent >()->SetWorldPosition( { 0.0f, -11.0f, 2.5f } );
		light.GetComponent< forge::PointLightComponent >()->GetData()->m_color = { 1.0f, 0.0f, 0.0f };
		light.GetComponent< forge::PointLightComponent >()->GetData()->m_power = 0.1f;
	} } );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ & ]( forge::Object& light, forge::ObjectInitData& )
	{
		light.AddComponents< forge::TransformComponent, forge::SpotLightComponent >();
		light.GetComponent< forge::TransformComponent >()->SetWorldPosition( { 0.0f, 0.0f, 25.0f } );
		light.GetComponent< forge::TransformComponent >()->SetWorldOrientation( Quaternion( -FORGE_PI_HALF, 0.0f, 0.0f ) );
		light.GetComponent< forge::SpotLightComponent >()->GetData()->m_color = { 0.5f, 0.5f, 0.5f };
	} } );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ & ]( forge::Object& light, forge::ObjectInitData& )
	{
		light.AddComponents< forge::TransformComponent, forge::PointLightComponent >();
		light.GetComponent< forge::TransformComponent >()->SetWorldPosition( { 0.0f, 11.0f, 2.5f } );
		light.GetComponent< forge::PointLightComponent >()->GetData()->m_color = { 0.0f, 0.0f, 1.0f };
		light.GetComponent< forge::PointLightComponent >()->GetData()->m_power = 0.1f;
	} } );
}

Int32 main()
{
	class GameInstance : public forge::ApplicationInstance
	{
	public:
		using forge::ApplicationInstance::ApplicationInstance;

		virtual void Initialize( forge::EngineInstance& engineInstance )
		{
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

			engineInstance.GetSystemsManager().GetSystem< systems::LightingSystem >().SetAmbientColor( { 0.55f, 0.55f, 0.55f } );

			engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ & ]( forge::Object& player, forge::ObjectInitData& )
			{
				player.AddComponents< forge::TransformComponent, forge::CameraComponent, forge::PhysicsFreeCameraControllerComponent >();
				player.GetComponent< forge::TransformComponent >()->SetWorldPosition( { 0.0f, 0.0f, 2.0f } );
				auto* cameraComp = player.GetComponent< forge::CameraComponent >();
				cameraComp->CreateImplementation< renderer::PerspectiveCamera >( forge::CameraComponent::GetDefaultPerspectiveCamera( engineInstance.GetRenderingManager().GetWindow() ));

				auto& camerasSystem = engineInstance.GetSystemsManager().GetSystem< systems::CamerasSystem >();
				camerasSystem.SetActiveCamera( cameraComp );

				auto* freeCameraController = player.GetComponent< forge::PhysicsFreeCameraControllerComponent >();
				engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController );
			} } );

			engineInstance.GetSystemsManager().GetSystem< systems::SceneRenderingSystem >().SetSkyboxTexture( engineInstance.GetAssetsManager().GetAsset< renderer::TextureAsset >( forge::Path( "Textures\\skymap.dds" ) )->GetTexture() );
			engineInstance.GetSystemsManager().GetSystem< systems::SceneRenderingSystem >().SetTargetTexture( &engineInstance.GetRenderingManager().GetRenderer().GetSwapchain()->GetBackBuffer() );

			SponzaScene( engineInstance );
			//SkeletalMesh( engineInstance, Vector3() );
		}

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{		
			if( engineInstance.GetRenderingManager().GetWindow().GetInput().GetKeyDown( forge::IInput::Key::Escape ) )
			{
				Shutdown();
			}

			if ( engineInstance.GetRenderingManager().GetWindow().GetInput().GetKeyDown( forge::IInput::Key::Space ) )
			{
				engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( { .m_postInitFunc = [ & ]( forge::Object& sphere, forge::ObjectInitData& )
				{
					sphere.AddComponents< forge::TransformComponent, forge::PhysicsDynamicComponent, forge::RenderingComponent >();
					auto* transformComponent = sphere.GetComponent< forge::TransformComponent >();
					auto* player = engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().GetCurrentPlayerObject();

					auto playerTransform = player->GetComponent< forge::TransformComponent >()->GetWorldTransform();

					transformComponent->SetWorldPosition( playerTransform.GetPosition3() + playerTransform.GetForward() );
					transformComponent->SetWorldScale( Vector3::ONES() * 0.5f );

					auto* renderingComponent = sphere.GetComponent< forge::RenderingComponent >();

					renderingComponent->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\sphere.obj" ) );
					renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", LinearColor{ Math::Random::GetRNG().GetFloat(), Math::Random::GetRNG().GetFloat(), Math::Random::GetRNG().GetFloat() } );
					renderingComponent->GetDirtyData()->m_renderable.GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();

					auto* physicsComponent = sphere.GetComponent< forge::PhysicsDynamicComponent >();
					physicsComponent->AddShape( physics::PhysicsShape( engineInstance.GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), 0.5f ) );
					physicsComponent->GetActor().AddForce( playerTransform.GetForward() * 25.0f, physics::PhysicsDynamicActor::ForceMode::VelocityChange );
					physicsComponent->GetActor().SetDensity( 100.0f );
				} } );
			}

			if ( auto* player = engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().GetCurrentPlayerObject() )
			{
				auto playerTransform = player->GetComponent< forge::TransformComponent >()->GetData()->m_transform;

				physics::RaycastResult result;
				if ( engineInstance.GetSystemsManager().GetSystem< systems::PhysicsSystem >().PerformRaycast( playerTransform.GetPosition3() + playerTransform.GetForward(), playerTransform.GetForward(), 100.0f, result ) )
				{
					engineInstance.GetSystemsManager().GetSystem< systems::DebugSystem >().DrawSphere( result.m_position, 0.5f, LinearColor::Red, true, false, -1.0f );
					FORGE_LOG( "Hit!" );
				}
			}

		}

		virtual Bool WithWindow() const override
		{
			return true;
		}

	private:
		forge::DirectionalLightComponent* m_sun = nullptr;

	} gameInstance( "Launcher" );

	forge::EngineInstance engineInstance( gameInstance );

	engineInstance.Run();
}