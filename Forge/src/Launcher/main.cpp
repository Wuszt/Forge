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
#include "../Systems/TimeSystem.h"
#include "../Renderer/TextureAsset.h"
#include "../Core/AssetsManager.h"
#include "../Renderer/FBXLoader.h"
#include "../../External/imgui/imgui.h"

std::string animName = "Animations\\Running.fbx";

void MinecraftScene( forge::EngineInstance& engineInstance )
{
	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( "Models\\vokselia\\vokselia_spawn.obj" );

			auto renderable = renderingComponent->GetData().m_renderable;

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

void SkeletalMesh( forge::EngineInstance& engineInstance )
{
	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			renderingComponent->LoadMeshAndMaterial( animName );

			//renderingComponent->GetRenderable()->GetMaterials()[ 0 ]->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 0.5f, 0.5f, 0.5f, 0.5f } );
			//renderingComponent->GetRenderable()->GetMaterials()[ 0 ]->SetRenderingPass( renderer::RenderingPass::Transparent );
			//renderingComponent->GetRenderable()->GetMaterials()[ 0 ]->GetConstantBuffer()->UpdateBuffer();
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

			renderingComponent->LoadMeshAndMaterial( "Models\\sponza\\sponza.obj" );

			auto renderable = renderingComponent->GetData().m_renderable;

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

	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			transformComponent->GetData().m_transform.SetPosition( { 0.0f, 0.0f, 200.0f } );
			transformComponent->GetData().m_scale = Vector3::ONES() * 250.0f;

			renderingComponent->LoadMeshAndMaterial( "Models\\sphere.obj" );

			auto renderable = renderingComponent->GetData().m_renderable;
			auto& material = renderable->GetMaterials()[0];
			material->SetShaders( "Uber.fx", "Uber.fx", renderer::RenderingPass::Transparent );
			material->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 1.0f, 0.0f, 0.0f, 0.1f } );
			material->GetConstantBuffer()->UpdateBuffer();
		} );
	} );

	engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* obj )
	{
		obj->RequestAddingComponents< forge::TransformComponent, forge::RenderingComponent >( [ engineInstancePtr = &engineInstance, obj ]()
		{
			auto* transformComponent = obj->GetComponent< forge::TransformComponent >();
			auto* renderingComponent = obj->GetComponent< forge::RenderingComponent >();

			transformComponent->GetData().m_transform.SetPosition( { 0.0f, 200.0f, 200.0f } );
			transformComponent->GetData().m_scale = Vector3::ONES() * 250.0f;

			renderingComponent->LoadMeshAndMaterial( "Models\\sphere.obj" );

			auto renderable = renderingComponent->GetData().m_renderable;
			auto& material = renderable->GetMaterials()[ 0 ];
			material->SetShaders( "Uber.fx", "Uber.fx", renderer::RenderingPass::Transparent );
			material->GetConstantBuffer()->SetData( "diffuseColor", Vector4{ 0.0f, 1.0f, 0.0f, 0.1f } );
			material->GetConstantBuffer()->UpdateBuffer();
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

			renderingComponent->LoadMeshAndMaterial( "Models\\bunny.obj" );

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

			renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );

			auto renderable = renderingComponent->GetData().m_renderable;
			for( auto& material : renderable->GetMaterials() )
			{
				material->SetTexture( engineInstancePtr->GetAssetsManager().GetAsset< renderer::TextureAsset >( "Textures\\grass.jpg" )->GetTexture(), renderer::Material::TextureType::Diffuse );
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

			renderingComponent->LoadMeshAndMaterial( "Models\\cube.obj" );

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

			renderingComponent->LoadMeshAndMaterial( "Models\\sphere.obj" );

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
		using forge::ApplicationInstance::ApplicationInstance;

		virtual void Initialize( forge::EngineInstance& engineInstance )
		{
			systems::SystemsManager::BootContext ctx;
			ctx.AddSystem< systems::CamerasSystem >();
			ctx.AddSystem< systems::PlayerSystem >();
			ctx.AddSystem< systems::RenderingSystem >();
			ctx.AddSystem< systems::LightingSystem >();
			ctx.AddSystem< systems::TimeSystem >();

#ifdef FORGE_DEBUGGING
			ctx.AddSystem< systems::DebugSystem >();
#endif

#ifdef FORGE_IMGUI_ENABLED
			ctx.AddSystem< systems::IMGUISystem >();
#endif

			engineInstance.GetSystemsManager().Boot( ctx );

			engineInstance.GetSystemsManager().GetSystem< systems::LightingSystem >().SetAmbientColor( { 0.55f, 0.55f, 0.55f } );

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
			//SponzaScene( engineInstance );
			//BunnyScene( engineInstance );	
			SkeletalMesh( engineInstance );

			engineInstance.GetEntitiesManager().RequestCreatingEntity< forge::Entity >( [ & ]( forge::Entity* light )
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
			Matrix Bones[100];
		};

		std::unique_ptr< renderer::StaticConstantBuffer< cbBones > > bonesBuffer;

		virtual void OnUpdate( forge::EngineInstance& engineInstance ) override
		{		
			accumulator += forge::Time::GetDeltaTime() * speed;
			frame = accumulator / 0.005f;


			if( engineInstance.GetWindow().GetInput()->GetKeyDown( forge::IInput::Key::Escape ) )
			{
				Shutdown();
			}

			if( m_sun )
			{
				Float currentAngle = DEG2RAD * 10.0f * engineInstance.GetSystemsManager().GetSystem< systems::TimeSystem >().GetCurrentTime();
				m_sun->GetData().Direction = Quaternion( -FORGE_PI_HALF, 0.0f, currentAngle ) * Vector3::EY();
				m_sun->GetData().Color = Vector3{ 1.0f, 1.0f, 1.0f } * Math::Cos( currentAngle );
			}

			{
				auto tmpAsset = engineInstance.GetAssetsManager().GetAsset<renderer::TMPAsset>( animName );

				frame %= tmpAsset->m_boneInfo[ 0 ].m_anim.size();

				static auto token = engineInstance.GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( [tmpAsset, this]()
				{
					ImGui::SliderInt( "Frame", &frame, 0, tmpAsset->m_boneInfo[ 0 ].m_anim.size() );
					ImGui::SliderFloat( "Speed", &speed, 0.0f, 100.0f );
				});

				for( Uint32 i = 0u; i < tmpAsset->m_boneInfo.size(); ++i )
				{
					tmpAsset->m_boneInfo[ i ].m_finalTransformation = tmpAsset->m_boneInfo[ i ].m_anim[ frame ];
				}

				//if(bonesBuffer == nullptr)
				{
					bonesBuffer = engineInstance.GetRenderer().CreateStaticConstantBuffer<cbBones>();
					
					for(Uint32 i = 0u; i < tmpAsset->m_boneInfo.size(); ++i)
					{
						bonesBuffer->GetData().Bones[ i ] = tmpAsset->m_boneInfo[ i ].m_finalTransformation;
					}
				}

				bonesBuffer->UpdateBuffer();
				bonesBuffer->SetVS(renderer::VSConstantBufferType::SkeletalMesh);
				
				for(Uint32 i = 0u; i < tmpAsset->m_boneInfo.size(); ++i)
				{
					Vector4 translation = tmpAsset->m_boneInfo[i].m_finalTransformation.GetTranslation();
					//engineInstance.GetSystemsManager().GetSystem<systems::DebugSystem>().DrawSphere(translation, 10.25f, Vector4(1.0f, 1.0f, 0.0f, 1.0f), -1.0f);
				}

				//std::function<void( const renderer::TMPAsset::Bone&, const Matrix& )> func;
				//func = [ & ]( const renderer::TMPAsset::Bone& bone, const Matrix& parentTransform )
				//{
				//	Matrix boneMatrix = bone.m_matrix;

				//	Vector3 pos = boneMatrix.GetTranslation();
				//	Vector3 rot = boneMatrix.ToEulerAngles() * RAD2DEG;

				//	{
				//		if( !bone.m_animation.empty() )
				//		{
				//			frame %= bone.m_animation.size();

				//			boneMatrix = bone.m_animation[ frame ];
				//		}
				//	}

				//	Matrix finalMatrix = boneMatrix * parentTransform;

				//	for( auto child : bone.m_children )
				//	{
				//		func( child, finalMatrix );
				//	}

				//	{
				//		if( bone.m_parent && bone.m_parent->m_parent )
				//		{
				//			Vector3 from = parentTransform.GetTranslation();
				//			Vector3 to = parentTransform.TransformPoint( boneMatrix.GetTranslation() );
				//			Vector3 direction = to - from;
				//			Float length = direction.Normalize();

				//			Float sphereRadius = 4.25f;
				//			Uint32 stepsAmount = static_cast<Uint32>( length / sphereRadius );

				//			for( Uint32 i = 0u; i < stepsAmount; ++i )
				//			{
				//				Vector3 pos = from + direction * sphereRadius * static_cast<Float>( i );
				//				//engineInstance.GetSystemsManager().GetSystem<systems::DebugSystem>().DrawSphere( pos, sphereRadius, Vector4( 0.0f, 1.0f, 0.0f, 1.0f ), -1.0f );
				//			}
				//		}
				//	}

				//	//engineInstance.GetSystemsManager().GetSystem<systems::DebugSystem>().DrawSphere( ( finalMatrix ).GetTranslation(), 5.25f, Vector4( 1.0f, 0.0f, 0.0f, 1.0f ), -1.0f );
				//};

				//func( tmpAsset->m_rootBone, Matrix::IDENTITY() );



				for(const Matrix& m : bonesBuffer->GetData().Bones)
				{
					//engineInstance.GetSystemsManager().GetSystem<systems::DebugSystem>().DrawSphere(m.GetTranslation(), 5.25f, Vector4(1.0f, 0.0f, 0.0f, 1.0f), -1.0f);
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