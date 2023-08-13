#include "../Core/PublicDefaults.h"
#include "../Math/PublicDefaults.h"
#include "../GameEngine/PublicDefaults.h"
#include "../Systems/PublicDefaults.h"

#include "../Core/IWindow.h"
#include "../Systems/CamerasSystem.h"
#include "../Systems/PlayerControllerComponent.h"
#include "../Systems/PlayerSystem.h"
#include "../Systems/CameraComponent.h"
#include "../Systems/SceneRenderingSystem.h"
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
#include "../GameEngine/ISystem.h"
#include "../Core/ArraySpan.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/AnimationComponent.h"
#include "../Systems/TransformSystem.h"
#include "../Physics/PhysxProxy.h"
#include "../Systems/PhysicsSystem.h"
#include "../Systems/PhysicsComponent.h"
#include "../Physics/PhysicsShape.h"
#include "../Renderer/ModelAsset.h"
#include "../Renderer/Model.h"
#include "../Physics/RaycastResult.h"
#include "../GameEngine/RenderingManager.h"

#ifdef FORGE_DEBUGGING
#include "../Systems/DebugSystem.h"
#endif

#ifdef FORGE_IMGUI_ENABLED
#include "../Systems/IMGUISystem.h"
#endif

#pragma optimize( "", off )
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

			transformComponent->GetDirtyTransform() = pos;
		} );
	} );
}

void SponzaScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent, forge::PhysicsStaticComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\sponza\\sponza.obj" );

			transformComponent->GetDirtyTransform().SetPosition( Vector3::ZEROS() );
			transformComponent->GetDirtyScale() = Vector3::ONES() * 0.01f;

			auto* physicsComponent = obj->GetComponent< forge::PhysicsStaticComponent >();
			auto modelAsset = engineInstancePtr->GetAssetsManager().GetAsset< renderer::ModelAsset >( "Models\\sponza\\sponza.obj" );

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
				physicsComponent->AddShape( physics::PhysicsShape( engineInstancePtr->GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), verts, shape.m_indices ) );
				++index;
			}
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetDirtyTransform().SetPosition( { 0.0f, -11.0f, 2.5f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 1.0f, 0.0f, 0.0f };
			light->GetComponent< forge::PointLightComponent >()->GetData().m_power = 0.1f;
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::SpotLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetDirtyTransform().SetPosition( { 0.0f, 0.0f, 25.0f } );
			light->GetComponent< forge::TransformComponent >()->GetDirtyTransform().SetOrientation( Quaternion( -FORGE_PI_HALF, 0.0f, 0.0f ) );
			light->GetComponent< forge::SpotLightComponent >()->GetData().m_color = { 0.5f, 0.5f, 0.5f };
		} );
	} );

	engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* light )
	{
		light->RequestAddingComponents< forge::TransformComponent, forge::PointLightComponent >( [ engineInstancePtr = &engineInstance, light ]()
		{
			light->GetComponent< forge::TransformComponent >()->GetDirtyTransform().SetPosition( { 0.0f, 11.0f, 2.5f } );
			light->GetComponent< forge::PointLightComponent >()->GetData().m_color = { 0.0f, 0.0f, 1.0f };
			light->GetComponent< forge::PointLightComponent >()->GetData().m_power = 0.1f;
		} );
	} );
}
#pragma optimize( "", on )

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
				&systems::SceneRenderingSystem::GetTypeStatic(),
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
					player->GetComponent< forge::TransformComponent >()->GetDirtyTransform().SetPosition( { 0.0f, 0.0f, 2.0f } );
					auto* cameraComp = player->GetComponent< forge::CameraComponent >();
					cameraComp->CreateImplementation< renderer::PerspectiveCamera >( forge::CameraComponent::GetDefaultPerspectiveCamera( engineInstancePtr->GetRenderingManager().GetWindow() ));

					auto& camerasSystem = engineInstancePtr->GetSystemsManager().GetSystem< systems::CamerasSystem >();
					camerasSystem.SetActiveCamera( cameraComp );

					auto* freeCameraController = player->GetComponent< forge::FreeCameraControllerComponent >();
					engineInstancePtr->GetSystemsManager().GetSystem< systems::PlayerSystem >().SetActivePlayerComponent( *freeCameraController );
				} );
			} );

			engineInstance.GetSystemsManager().GetSystem< systems::SceneRenderingSystem >().SetSkyboxTexture( engineInstance.GetAssetsManager().GetAsset< renderer::TextureAsset >( "Textures\\skymap.dds" )->GetTexture() );

			SponzaScene( engineInstance );
			//SkeletalMesh( engineInstance, { 0.0f, 400.0f, 0.0f } );
			//SkeletalMesh( engineInstance, { 0.0f, 600.0f, 0.0f } );
			//SkeletalMesh( engineInstance, { 0.0f, 800.0f, 0.0f } );
		}

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{		
			if( engineInstance.GetRenderingManager().GetWindow().GetInput()->GetKeyDown( forge::IInput::Key::Escape ) )
			{
				Shutdown();
			}

			if ( engineInstance.GetRenderingManager().GetWindow().GetInput()->GetKeyDown( forge::IInput::Key::Space ) )
			{
				engineInstance.GetObjectsManager().RequestCreatingObject< forge::Object >( [ & ]( forge::Object* sphere )
					{
						sphere->RequestAddingComponents< forge::TransformComponent, forge::PhysicsDynamicComponent, forge::RenderingComponent >( [ sphere, &engineInstance ]()
							{
								auto* transformComponent = sphere->GetComponent< forge::TransformComponent >();
								auto* player = engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().GetCurrentPlayerObject();

								auto playerTransform = player->GetComponent< forge::TransformComponent >()->GetData().m_transform;

								transformComponent->GetDirtyTransform().SetPosition( playerTransform.GetPosition3() + playerTransform.GetForward() );
								transformComponent->GetDirtyScale() = Vector3::ONES() * 0.5f;

								auto* renderingComponent = sphere->GetComponent< forge::RenderingComponent >();

								renderingComponent->LoadMeshAndMaterial( "Models\\sphere.obj" );
								renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ Math::Random::GetRNG().GetFloat(), Math::Random::GetRNG().GetFloat(), Math::Random::GetRNG().GetFloat(), 1.0f } );
								renderingComponent->GetDirtyRenderable().GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();

								auto* physicsComponent = sphere->GetComponent< forge::PhysicsDynamicComponent >();
								physicsComponent->AddShape( physics::PhysicsShape( engineInstance.GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), 0.5f ) );
								physicsComponent->GetActor().AddForce( playerTransform.GetForward() * 25.0f, physics::PhysicsDynamicActor::ForceMode::VelocityChange );
								physicsComponent->GetActor().UpdateDensity( 100.0f );
							} );
					} );
			}

			if ( auto* player = engineInstance.GetSystemsManager().GetSystem< systems::PlayerSystem >().GetCurrentPlayerObject() )
			{
				auto playerTransform = player->GetComponent< forge::TransformComponent >()->GetData().m_transform;

				physics::RaycastResult result;
				if ( engineInstance.GetSystemsManager().GetSystem< systems::PhysicsSystem >().PerformRaycast( playerTransform.GetPosition3() + playerTransform.GetForward(), playerTransform.GetForward(), 100.0f, result ) )
				{
					engineInstance.GetSystemsManager().GetSystem< systems::DebugSystem >().DrawSphere( result.m_position, 0.5f, Vector4( 1.0f, 0.0f, 0.0f, 1.0f ), true, false, -1.0f );
					FORGE_LOG( "Hit!" );
				}
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