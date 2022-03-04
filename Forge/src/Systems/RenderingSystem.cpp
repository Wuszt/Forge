#include "Fpch.h"
#include "../Renderer/PublicDefaults.h"
#include "RenderingSystem.h"

#include "CamerasSystem.h"
#include "CameraComponent.h"
#include "LightingSystem.h"
#include "../Renderer/ForwardRenderingPass.h"
#include "../Renderer/DefferedRenderingPass.h"
#include "../Renderer/IDepthStencilState.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../../External/imgui/imgui.h"
#include "../Renderer/FullScreenRenderingPass.h"
#include "../Renderer/ICamera.h"
#endif

systems::RenderingSystem::RenderingSystem( forge::EngineInstance& engineInstance )
	: ECSSystem< forge::TransformComponentData, forge::RenderingComponentData >( engineInstance )
{}

systems::RenderingSystem::~RenderingSystem() = default;

void systems::RenderingSystem::OnInitialize()
{
	m_renderer = &GetEngineInstance().GetRenderer();
	m_camerasSystem = &GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >();
	
	m_beforeDrawToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, std::bind( &systems::RenderingSystem::OnBeforeDraw, this ) );
	m_drawToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Rendering, std::bind( &systems::RenderingSystem::OnDraw, this ) );
	m_presentToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Present, std::bind( &systems::RenderingSystem::OnPresent, this ) );
	m_cameraCB = m_renderer->CreateStaticConstantBuffer< renderer::cbCamera >();
	m_rawRenderablesPacks = m_renderer->CreateRawRenderablesPackage( {} );

	m_opaqueRenderingPass = std::make_unique< renderer::DefferedRenderingPass >( *m_renderer, [ cameraSystem = m_camerasSystem ]() -> decltype(auto) { return cameraSystem->GetActiveCamera()->GetCamera(); } );
	//m_opaqueRenderingPass = std::make_unique< renderer::ForwardRenderingPass >( *m_renderer );
	m_opaqueRenderingPass->SetTargetTexture( m_renderer->GetSwapchain()->GetBackBuffer() );
	m_opaqueRenderingPass->SetDepthStencilBuffer( m_renderer->GetDepthStencilBuffer() );

	m_overlayRenderingPass = std::make_unique< renderer::ForwardRenderingPass >( *m_renderer );
	m_overlayRenderingPass->SetTargetTexture( m_renderer->GetSwapchain()->GetBackBuffer() );
	m_overlayRenderingPass->SetDepthStencilBuffer( m_renderer->GetDepthStencilBuffer() );

	std::vector< renderer::ShaderDefine > baseShaderDefines;
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::DefferedRenderingPass::GetRequiredShaderDefines().begin(), renderer::DefferedRenderingPass::GetRequiredShaderDefines().end() );
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::ForwardRenderingPass::GetRequiredShaderDefines().begin(), renderer::ForwardRenderingPass::GetRequiredShaderDefines().end() );
	m_renderer->GetShadersManager()->SetBaseShaderDefines( std::move( baseShaderDefines ) );

	m_depthStencilState = m_renderer->CreateDepthStencilState( renderer::DepthStencilComparisonFunc::COMPARISON_LESS_EQUAL );
	m_depthStencilState->Set();

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
					material->SetShaders( material->GetVertexShaderPath(), material->GetPixelShaderPath(), renderer::RenderingPass::Opaque );
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
#endif
}

#ifdef FORGE_IMGUI_ENABLED
void systems::RenderingSystem::OnRenderDebug()
{
	ImGui::ShowDemoWindow();
	ImGui::Begin( "RenderingDebug" );

	Int32 currentMode = static_cast<Int32>( m_renderingMode );
	ImGui::Text( "Rendering mode: " );
	ImGui::SameLine(); ImGui::RadioButton( "Forward", &currentMode, 0 );
	ImGui::SameLine(); ImGui::RadioButton( "Deffered", &currentMode, 1 );
	if( currentMode != static_cast<Int32>( m_renderingMode ) )
	{
		SetRenderingMode( static_cast<RenderingMode>( currentMode ) );
	}

	auto funcDrawTexture = []( renderer::ITexture& texture )
	{
		const Vector2& size = texture.GetTextureSize();
		const ImVec2 textureSize = ImVec2( ImGui::GetWindowWidth(), ImGui::GetWindowWidth() * size.Y / size.X );
		ImGui::Image( texture.GetShaderResourceView()->GetRawSRV(), textureSize );
	};

	{
		if( ImGui::BeginTabBar( "##tabs" ) )
		{
			if( ImGui::BeginTabItem( "Loaded textures" ) )
			{
				auto loadedTextures = m_renderer->GetResourceManager().GetAllLoadedTextures();
				for( const auto& texture : loadedTextures )
				{
					funcDrawTexture( *texture );
				}
				ImGui::EndTabItem();
			}

			if( dynamic_cast<renderer::DefferedRenderingPass*>( m_opaqueRenderingPass.get() ) )
			{
				if( ImGui::BeginTabItem( "GBuffer" ) )
				{
					funcDrawTexture( *static_cast<renderer::DefferedRenderingPass*>( m_opaqueRenderingPass.get() )->GetNormalsTexture() );
					funcDrawTexture( *static_cast<renderer::DefferedRenderingPass*>( m_opaqueRenderingPass.get() )->GetDiffuseTexture() );
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

				m_temporaryTexture = m_renderer->CreateTexture( static_cast<Uint32>( m_renderer->GetResolution().X ), static_cast<Uint32>( m_renderer->GetResolution().Y ),
					renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE,
					renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Format::R8G8B8A8_UNORM );

				fsPass.SetTargetTexture( *m_temporaryTexture );
				fsPass.Draw( { m_renderer->GetDepthStencilBuffer()->GetTexture()->GetShaderResourceView() } );
				funcDrawTexture( *m_temporaryTexture );
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void systems::RenderingSystem::SetRenderingMode( RenderingMode renderingMode )
{
	m_renderingMode = renderingMode;
	switch( renderingMode )
	{
	case RenderingMode::Deffered:
		m_opaqueRenderingPass = std::make_unique< renderer::DefferedRenderingPass >( *m_renderer, [ cameraSystem = m_camerasSystem ]() -> decltype( auto ) { return cameraSystem->GetActiveCamera()->GetCamera(); } );
		break;
	case RenderingMode::Forward:
		m_opaqueRenderingPass = std::make_unique< renderer::ForwardRenderingPass >( *m_renderer );
		break;
	}
	m_opaqueRenderingPass->SetTargetTexture( m_renderer->GetSwapchain()->GetBackBuffer() );
	m_opaqueRenderingPass->SetDepthStencilBuffer( m_renderer->GetDepthStencilBuffer() );
}

#endif

void systems::RenderingSystem::OnBeforeDraw()
{
	m_renderer->OnBeforeDraw();
	m_opaqueRenderingPass->ClearTargetTexture(); // this is fucked up, what about other rendering passes?
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

	std::vector< const renderer::Renderable* > renderables;
	for( systems::Archetype* archetype : archetypes )
	{
		const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
		const forge::DataPackage< forge::RenderingComponentData >& renderableComponents = archetype->GetData< forge::RenderingComponentData >();

		for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
		{
			renderables.emplace_back( renderableComponents[ i ].m_renderable );
		}
	}

	m_rawRenderablesPacks = m_renderer->CreateRawRenderablesPackage( renderables );

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

	auto& lightingSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::LightingSystem >();
	renderer::LightingData lightingData = { lightingSystem.GetAmbientColor(), lightingSystem.GetLights() };
	m_opaqueRenderingPass->Draw( m_rawRenderablesPacks->GetRendenderablesPack( renderer::RenderingPass::Opaque ), &lightingData );
	m_overlayRenderingPass->Draw( m_rawRenderablesPacks->GetRendenderablesPack( renderer::RenderingPass::Overlay ), nullptr );
}

void systems::RenderingSystem::OnPresent()
{
	m_renderer->GetSwapchain()->Present();
}

void systems::RenderingSystem::SetSamplers( const forge::ArraySpan< const renderer::SamplerStateFilterType >& filterTypes )
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
