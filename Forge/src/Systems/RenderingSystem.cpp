#include "Fpch.h"
#include "../Renderer/PublicDefaults.h"
#include "RenderingSystem.h"

#include "CamerasSystem.h"
#include "CameraComponent.h"
#include "LightingSystem.h"
#include "../Renderer/ForwardRenderingPass.h"
#include "../Renderer/DeferredRenderingPass.h"
#include "../Renderer/IDepthStencilBuffer.h"
#include "../Renderer/IDepthStencilState.h"
#include "../Core/IWindow.h"
#include "../Renderer/FullScreenRenderingPass.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../../External/imgui/imgui.h"
#include "../Renderer/ICamera.h"
#endif
#include "../Renderer/ShadowMapsGenerator.h"

systems::RenderingSystem::RenderingSystem( forge::EngineInstance& engineInstance )
	: ECSSystem< systems::ArchetypeDataTypes< forge::TransformComponentData, forge::RenderingComponentData > >( engineInstance )
{}

systems::RenderingSystem::~RenderingSystem()
{
	m_opaqueRenderingPass = nullptr;
	m_overlayRenderingPass = nullptr;
}

void systems::RenderingSystem::OnInitialize()
{
	m_renderer = &GetEngineInstance().GetRenderer();
	m_camerasSystem = &GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >();
	m_depthStencilBuffer = m_renderer->CreateDepthStencilBuffer( GetEngineInstance().GetWindow().GetWidth(), GetEngineInstance().GetWindow().GetHeight() );
	m_shadowMapsGenerator = std::make_unique<renderer::ShadowMapsGenerator>( *m_renderer );

	{
		m_samplerStates.emplace_back( m_renderer->CreateSamplerState( renderer::SamplerStateFilterType::MIN_MAG_MIP_LINEAR, renderer::SamplerStateComparisonType::ALWAYS ) );
		m_samplerStates.emplace_back( m_renderer->CreateSamplerState( renderer::SamplerStateFilterType::COMPARISON_MIN_MAG_LINEAR_MIP_POINT, renderer::SamplerStateComparisonType::LESS ) );

		std::vector< renderer::ISamplerState* > samplerStates;
		for( auto& samplerState : m_samplerStates )
		{
			samplerStates.emplace_back( samplerState.get() );
		}

		m_renderer->SetSamplerStates( samplerStates );
	}

	m_beforeDrawToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, std::bind( &systems::RenderingSystem::OnBeforeDraw, this ) );
	m_drawToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Rendering, std::bind( &systems::RenderingSystem::OnDraw, this ) );
	m_presentToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Present, std::bind( &systems::RenderingSystem::OnPresent, this ) );
	m_rawRenderablesPacks = m_renderer->CreateRawRenderablesPackage( {} );

	m_targetTexture = m_renderer->CreateTexture( GetEngineInstance().GetWindow().GetWidth(), GetEngineInstance().GetWindow().GetHeight(), 
		renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE,
		renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );

	SetRenderingMode( RenderingMode::Deferred );

	m_overlayRenderingPass = std::make_unique< renderer::ForwardRenderingPass >( *m_renderer );
	m_overlayRenderingPass->SetTargetTexture( *m_targetTexture );
	m_overlayRenderingPass->SetDepthStencilBuffer( m_depthStencilBuffer.get() );

	std::vector< renderer::ShaderDefine > baseShaderDefines;
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::DeferredRenderingPass::GetRequiredShaderDefines().begin(), renderer::DeferredRenderingPass::GetRequiredShaderDefines().end() );
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::ForwardRenderingPass::GetRequiredShaderDefines().begin(), renderer::ForwardRenderingPass::GetRequiredShaderDefines().end() );
	m_renderer->GetShadersManager()->SetBaseShaderDefines( std::move( baseShaderDefines ) );

	m_depthStencilState = m_renderer->CreateDepthStencilState( renderer::DepthStencilComparisonFunc::COMPARISON_LESS_EQUAL );
	m_depthStencilState->Set();
	m_windowCallbackToken = GetEngineInstance().GetWindow().RegisterEventListener(
			[ & ]( const forge::IWindow::IEvent& event )
	{
		switch( event.GetEventType() )
		{
		case forge::IWindow::EventType::OnResized:
			FORGE_ASSERT( dynamic_cast<const forge::IWindow::OnResizedEvent*>( &event ) );
			const forge::IWindow::OnResizedEvent& resizedEvent = static_cast<const forge::IWindow::OnResizedEvent&>( event );

			UpdateRenderingResolution( m_renderingResolutionScale );
			break;
		}
	} );

#ifdef FORGE_IMGUI_ENABLED
	m_overlayDebugToken = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( [ this ]()
	{
		if( ImGui::Button( "Reload shaders" ) )
		{
			GetEngineInstance().GetRenderer().GetShadersManager()->ClearCache();
		}

		ImGui::Text( "Window res: %u x %u", GetEngineInstance().GetWindow().GetWidth(), GetEngineInstance().GetWindow().GetHeight() );

		const Vector2 renderingRes = GetRenderingResolution();
		ImGui::Text( "Rendering res(%u%%) : %u x %u", static_cast<Uint32>( m_renderingResolutionScale * 100.0f ), static_cast< Uint32 >( renderingRes.X ), static_cast< Uint32 >( renderingRes.Y ) );
	} );
#endif
}

#ifdef FORGE_IMGUI_ENABLED
void systems::RenderingSystem::OnRenderDebug()
{
	if( ImGui::Begin( "Rendering System" ) )
	{
		if( ImGui::BeginTabBar( "##tabs" ) )
		{
			if( ImGui::BeginTabItem( "Settings" ) )
			{
				Int32 currentMode = static_cast<Int32>( m_renderingMode );
				ImGui::Text( "Rendering mode: " );
				ImGui::SameLine(); ImGui::RadioButton( "Forward", &currentMode, 0 );
				ImGui::SameLine(); ImGui::RadioButton( "Deferred", &currentMode, 1 );
				if( currentMode != static_cast<Int32>( m_renderingMode ) )
				{
					SetRenderingMode( static_cast<RenderingMode>( currentMode ) );
				}

				Int32 scale = static_cast< Int32 >( m_renderingResolutionScale * 100.0f );
				if( ImGui::SliderInt( "##ResolutionScale", &scale, 1, 800, "Resolution Scale: %d%%" ) )
				{
					UpdateRenderingResolution( static_cast<Float>( scale ) * 0.01f );
				}

				ImGui::EndTabItem();
			}

			auto funcDrawTexture = []( const std::string& name, renderer::ITexture& texture )
			{
				if( ImGui::TreeNodeEx( name.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					const Vector2& size = texture.GetTextureSize();
					const ImVec2 textureSize = ImVec2( ImGui::GetWindowWidth(), ImGui::GetWindowWidth() * size.Y / size.X );
					ImGui::Image( texture.GetShaderResourceView()->GetRawSRV(), textureSize );
					if( ImGui::IsItemHovered() && ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
					{
						ImVec2 pos = ImGui::GetCursorScreenPos();
						ImGuiIO& io = ImGui::GetIO();
						ImGui::BeginTooltip();
						Float region_sz = 32.0f;
						Float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
						Float region_y = textureSize.y - Math::Abs( io.MousePos.y - pos.y - region_sz * 0.5f );
						Float zoom = 4.0f;
						Math::Clamp( 0.0f, textureSize.x - region_sz, region_x );
						Math::Clamp( 0.0f, textureSize.y - region_sz, region_y );
						ImVec2 uv0 = ImVec2( ( region_x ) / textureSize.x, ( region_y ) / textureSize.y );
						ImVec2 uv1 = ImVec2( ( region_x + region_sz ) / textureSize.x, ( region_y + region_sz ) / textureSize.y );
						ImGui::Image( texture.GetShaderResourceView()->GetRawSRV(), ImVec2( region_sz * zoom, region_sz * zoom ), uv0, uv1 );
						ImGui::EndTooltip();
					}
					ImGui::TreePop();
				}
			};

			if( ImGui::BeginTabItem( "Loaded textures" ) )
			{
				for( const auto& texture : m_renderer->GetResourceManager().GetAllLoadedTextures() )
				{
					funcDrawTexture( texture.first, *texture.second );
				}

				ImGui::EndTabItem();
			}

			if( ImGui::BeginTabItem( "Buffers" ) )
			{
				if( dynamic_cast<renderer::DeferredRenderingPass*>( m_opaqueRenderingPass.get() ) )
				{
					funcDrawTexture( "Normals", *static_cast<renderer::DeferredRenderingPass*>( m_opaqueRenderingPass.get() )->GetNormalsTexture() );
					funcDrawTexture( "Diffuse", *static_cast<renderer::DeferredRenderingPass*>( m_opaqueRenderingPass.get() )->GetDiffuseTexture() );
				}

				{
					const renderer::ICamera& currentCamera = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera();
					Float maxValue = currentCamera.GetType() == renderer::ICamera::Type::Perspective ? currentCamera.GetFarPlane() - currentCamera.GetNearPlane() : 1.0f;
					m_depthBufferDenominator = Math::Min( m_depthBufferDenominator, maxValue );
					ImGui::SliderFloat( "Denominator", &m_depthBufferDenominator, maxValue * 0.001f, maxValue );

					struct CB
					{
						Float Denominator = 1.0f;
						Float padding[ 3 ];
					};

					auto cb = m_renderer->CreateStaticConstantBuffer< CB >();
					cb->GetData().Denominator = m_depthBufferDenominator;
					cb->UpdateBuffer();
					cb->SetPS( renderer::PSConstantBufferType::Material );

					m_temporaryTexture = m_renderer->CreateTexture( GetEngineInstance().GetWindow().GetWidth(), GetEngineInstance().GetWindow().GetHeight(),
						renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE,
						renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );

					renderer::FullScreenRenderingPass fsPass( *m_renderer, "DepthBufferDebug.fx", currentCamera.HasNonLinearDepth() ? forge::ArraySpan< renderer::ShaderDefine >{ renderer::ShaderDefine{ "__NON_LINEAR_DEPTH__" } } : forge::ArraySpan< renderer::ShaderDefine >{} );
					fsPass.SetTargetTexture( *m_temporaryTexture );
					fsPass.Draw( { m_depthStencilBuffer->GetTexture()->GetShaderResourceView() } );
					funcDrawTexture( "Depth", *m_temporaryTexture );
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

#endif

void systems::RenderingSystem::SetRenderingMode( RenderingMode renderingMode )
{
	m_renderingMode = renderingMode;
	switch( renderingMode )
	{
	case RenderingMode::Deferred:
		m_opaqueRenderingPass = std::make_unique< renderer::DeferredRenderingPass >( *m_renderer );
		break;
	case RenderingMode::Forward:
		m_opaqueRenderingPass = std::make_unique< renderer::ForwardRenderingPass >( *m_renderer );
		break;
	}
	m_opaqueRenderingPass->SetTargetTexture( *m_targetTexture );
	m_opaqueRenderingPass->SetDepthStencilBuffer( m_depthStencilBuffer.get() );
}

void systems::RenderingSystem::UpdateRenderingResolution( Float scale )
{
	m_renderingResolutionScale = scale;
	const Vector2 renderingResolution = GetRenderingResolution();

	const Uint32 renderingResolutionWidth = static_cast<Uint32>( renderingResolution.X );
	const Uint32 renderingResolutionHeight = static_cast<Uint32>( renderingResolution.Y );
	m_depthStencilBuffer->Resize( renderingResolutionWidth, renderingResolutionHeight );
	m_targetTexture->Resize( renderingResolution );
}

Vector2 systems::RenderingSystem::GetRenderingResolution()
{
	Vector2 result = { static_cast<Float>( GetEngineInstance().GetWindow().GetWidth() ), static_cast<Float>( GetEngineInstance().GetWindow().GetHeight() ) };
	result *= m_renderingResolutionScale;
	result.X = static_cast< Float >( static_cast< Uint32 >( result.X ) );
	result.Y = static_cast< Float >( static_cast< Uint32 >( result.Y ) );

	return result;
}

void systems::RenderingSystem::OnBeforeDraw()
{
	m_renderer->OnBeforeDraw();
	m_opaqueRenderingPass->ClearTargetTexture(); // this is fucked up, what about other rendering passes?
}

void systems::RenderingSystem::OnDraw()
{
	PC_SCOPE_FUNC();
	auto* activeCamera = m_camerasSystem->GetActiveCamera();

	if( !activeCamera )
	{
		return;
	}

	const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( systems::ArchetypeDataTypes< forge::TransformComponentData, forge::RenderingComponentData >() );
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
	renderer::LightingData lightingData = lightingSystem.GetLightingData();
	m_shadowMapsGenerator->GenerateShadowMaps( *m_rawRenderablesPacks, lightingData );

	m_opaqueRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), m_rawRenderablesPacks->GetRendenderablesPack( renderer::RenderingPass::Opaque ), &lightingData );
	m_overlayRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), m_rawRenderablesPacks->GetRendenderablesPack( renderer::RenderingPass::Overlay ), nullptr );

	m_renderer->SetViewportSize( Vector2( static_cast< Float >( GetEngineInstance().GetWindow().GetWidth() ), static_cast<Float>( GetEngineInstance().GetWindow().GetHeight() ) ) );
	renderer::FullScreenRenderingPass copyResourcePass( *m_renderer, "CopyTexture.fx", {} );
	copyResourcePass.SetTargetTexture( m_renderer->GetSwapchain()->GetBackBuffer() );
	copyResourcePass.Draw( { m_targetTexture->GetShaderResourceView() } );
}

void systems::RenderingSystem::OnPresent()
{
	m_renderer->GetSwapchain()->Present();
}
