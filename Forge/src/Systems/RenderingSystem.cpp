#include "Fpch.h"
#include "../Renderer/PublicDefaults.h"
#include "RenderingSystem.h"
#include "../Renderer/IRenderer.h"
#include "CamerasSystem.h"
#include "CameraComponent.h"
#include "../Renderer/ConstantBuffer.h"
#include "../Renderer/Renderable.h"
#include "../Renderer/ForwardRenderingPass.h"
#include "../Renderer/DefferedRenderingPass.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../../External/imgui/imgui.h"
#include "../Renderer/FullScreenRenderingPass.h"
#include "../Renderer/ICamera.h"
#endif

void systems::RenderingSystem::OnInitialize()
{
	m_renderer = &GetEngineInstance().GetRenderer();
	m_camerasSystem = &GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >();
	
	m_beforeDrawToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, std::bind( &systems::RenderingSystem::OnBeforeDraw, this ) );
	m_drawToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Rendering, std::bind( &systems::RenderingSystem::OnDraw, this ) );
	m_presentToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Present, std::bind( &systems::RenderingSystem::OnPresent, this ) );
	m_cameraCB = m_renderer->CreateStaticConstantBuffer< renderer::cbCamera >();
	m_rawRenderablesPackage = m_renderer->CreateRawRenderablesPackage( {} );
	m_opaqueRenderingPass = std::make_unique< renderer::DefferedRenderingPass >( *m_renderer );
	m_opaqueRenderingPass->SetRenderTargetView( m_renderer->GetRenderTargetView() );
	m_opaqueRenderingPass->SetDepthStencilBuffer( m_renderer->GetDepthStencilBuffer() );

#ifdef FORGE_DEBUGGING
	m_clearingCacheToken = m_renderer->GetShadersManager()->RegisterCacheClearingListener( [ this ]()
	{
		const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesOfSystem< systems::RenderingSystem >();
		for( systems::Archetype* archetype : archetypes )
		{
			forge::DataPackage< forge::RenderingComponentData >& renderableComponents = archetype->GetData< forge::RenderingComponentData >();
			for( Uint32 i = 0; i < renderableComponents.GetDataSize(); ++i )
			{
				auto& materials = renderableComponents[ i ].m_renderable->GetMaterials();
				for( auto& material : materials )
				{
					material.SetVertexShader( material.GetVertexShaderPath() );
					material.SetPixelShader( material.GetPixelShaderPath() );
				}
			}
		}
	} );
#endif

#ifdef FORGE_IMGUI_ENABLED
	m_overlayDebugToken = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( [ this ]()
	{
		if( ImGui::Button( "Reload shaders" ) )
		{
			GetEngineInstance().GetRenderer().GetShadersManager()->ClearCache();
		}
	} );

	renderer::ITexture::Flags flags = renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE;
	std::shared_ptr< renderer::ITexture > texture = m_renderer->CreateTexture( static_cast<Uint32>( m_renderer->GetResolution().X ), static_cast<Uint32>( m_renderer->GetResolution().Y ), flags, renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Format::R8G8B8A8_UNORM );

	m_temporaryRTV = m_renderer->CreateRenderTargetView( texture );
#endif
}

#ifdef FORGE_IMGUI_ENABLED
void systems::RenderingSystem::OnRenderDebug()
{
	ImGui::ShowDemoWindow();
	ImGui::Begin( "RenderingDebug" );

	{
		Float aspectRatio = 9.0f / 16.0f;
		ImVec2 textureSize = ImVec2( ImGui::GetWindowWidth(), ImGui::GetWindowWidth() * aspectRatio );
		if( ImGui::BeginTabBar( "##tabs" ) )
		{
			if( ImGui::BeginTabItem( "Loaded textures" ) )
			{
				auto loadedTextures = m_renderer->GetResourceManager().GetAllLoadedTextures();
				for( const auto& texture : loadedTextures )
				{
					ImGui::Image( texture->GetShaderResourceView()->GetRawSRV(), textureSize );
				}
				ImGui::EndTabItem();
			}

			if( dynamic_cast<renderer::DefferedRenderingPass*>( m_opaqueRenderingPass.get() ) )
			{
				if( ImGui::BeginTabItem( "GBuffer" ) )
				{
					ImGui::Image( static_cast<renderer::DefferedRenderingPass*>( m_opaqueRenderingPass.get() )->GetNormalsTargetView()->GetTexture()->GetShaderResourceView()->GetRawSRV(), textureSize );
					ImGui::Image( static_cast<renderer::DefferedRenderingPass*>( m_opaqueRenderingPass.get() )->GetDiffuseTargetView()->GetTexture()->GetShaderResourceView()->GetRawSRV(), textureSize );
					ImGui::EndTabItem();
				}
			}

			if( ImGui::BeginTabItem( "Depth Buffer" ) )
			{
				renderer::FullScreenRenderingPass fsPass( *m_renderer, "DepthBufferDebug.fx" );

				struct CB
				{
					Float Denominator = 1.0f;
					Float padding[3];
				};

				auto cb = m_renderer->CreateStaticConstantBuffer< CB >();
				const forge::ICamera& currentCamera = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera();
				Float maxValue = currentCamera.GetFarPlane() - currentCamera.GetNearPlane();
				m_depthBufferDenominator = Math::Min( m_depthBufferDenominator, maxValue );
				ImGui::SliderFloat( "Denominator", &m_depthBufferDenominator, maxValue * 0.1f, maxValue);
				cb->GetData().Denominator = m_depthBufferDenominator;
				cb->UpdateBuffer();
				cb->SetPS( renderer::PSConstantBufferType::Material );
				fsPass.SetRenderTargetView( m_temporaryRTV.get() );
				fsPass.Draw( { m_renderer->GetDepthStencilBuffer()->GetTexture()->GetShaderResourceView() } );
				ImGui::Image( m_temporaryRTV->GetTexture()->GetShaderResourceView()->GetRawSRV(), textureSize );
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}
#endif

void systems::RenderingSystem::OnBeforeDraw()
{
	m_renderer->BeginScene();
	m_opaqueRenderingPass->ClearRenderTargetView();
}

void systems::RenderingSystem::OnDraw()
{
	PC_SCOPE_FUNC();

	const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesOfSystem< systems::RenderingSystem >();

	auto* activeCamera = m_camerasSystem->GetActiveCamera();

	if( !activeCamera )
	{
		return;
	}

	m_cameraCB->GetData().VP = activeCamera->GetCamera().GetViewProjectionMatrix();
	m_cameraCB->GetData().CameraPosition = activeCamera->GetCamera().GetPosition();
	m_cameraCB->GetData().CameraDirection = activeCamera->GetCamera().GetOrientation() * Vector3::EY();
	m_cameraCB->GetData().ProjectionA = activeCamera->GetCamera().GetProjectionMatrix()[ 1 ][ 2 ];
	m_cameraCB->GetData().ProjectionB = activeCamera->GetCamera().GetProjectionMatrix()[ 3 ][ 2 ];
	m_cameraCB->UpdateBuffer();
	m_cameraCB->SetVS( renderer::VSConstantBufferType::Camera );
	m_cameraCB->SetPS( renderer::PSConstantBufferType::Camera );

	m_rawRenderablesPackage = nullptr;
	for( systems::Archetype* archetype : archetypes )
	{
		const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
		const forge::DataPackage< forge::RenderingComponentData >& renderableComponents = archetype->GetData< forge::RenderingComponentData >();

		std::vector< const renderer::Renderable* > renderables;

		for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
		{
			renderables.emplace_back( renderableComponents[ i ].m_renderable );
		}

		m_rawRenderablesPackage = m_renderer->CreateRawRenderablesPackage( renderables );
	}

	for( systems::Archetype* archetype : archetypes )
	{
		const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
		const forge::DataPackage< forge::RenderingComponentData >& renderableComponents = archetype->GetData< forge::RenderingComponentData >();

		for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
		{
			auto& cb = renderableComponents[ i ].m_renderable->GetCBMesh();
			auto prev = cb.GetData().W;
			cb.GetData().W = transformComponents[ i ].ToMatrix();

			// It's stupid, but UpdateBuffer takes so much time that it makes sense for now
			// I should probably separate static and dynamic objects in the future or/and keep an information which entity changed
			// it's transform somehow
			if( prev != cb.GetData().W )
			{
				cb.UpdateBuffer();
			}
		}
	}

	if( m_rawRenderablesPackage )
	{
		m_opaqueRenderingPass->Draw( *m_rawRenderablesPackage );
	}
}

void systems::RenderingSystem::OnPresent()
{
	m_renderer->GetSwapchain()->Present();
}

void systems::RenderingSystem::SetSamplers( const std::vector< renderer::SamplerStateFilterType >& filterTypes )
{
	std::vector< renderer::ISamplerState* > samplerStates;
	for( auto filterType : filterTypes )
	{
		auto it = m_samplerStates.find( filterType );
		if( it == m_samplerStates.end() )
		{
			it = m_samplerStates.emplace( filterType, m_renderer->CreateSamplerState( filterType ) ).first;
		}

		samplerStates.push_back( it->second.get() );
	}

	m_renderer->SetSamplerStates( samplerStates );
}
