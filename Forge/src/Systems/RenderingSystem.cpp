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
#include "../Renderer/IBlendState.h"
#include "../Renderer/ShadowMapsGenerator.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../IMGUI/PublicDefaults.h"
#include "../Renderer/ICamera.h"
#endif
#include "../Renderer/TextureAsset.h"
#include "../Core/AssetsManager.h"

IMPLEMENT_TYPE( systems, RenderingSystem );

systems::RenderingSystem::RenderingSystem()
	: ECSSystem< systems::ArchetypeDataTypes< forge::TransformComponentData, forge::RenderingComponentData > >()
{}

systems::RenderingSystem::~RenderingSystem() = default;

void systems::RenderingSystem::OnInitialize()
{
#ifdef FORGE_IMGUI_ENABLED
	InitializeDebuggable< systems::RenderingSystem >( GetEngineInstance() );
#endif

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

	m_transparentRenderingPass = std::make_unique< renderer::ForwardRenderingPass >( *m_renderer );
	m_transparentRenderingPass->SetTargetTexture( *m_targetTexture );
	m_transparentRenderingPass->SetDepthStencilBuffer( m_depthStencilBuffer.get() );

	m_transparencyBlendState = GetEngineInstance().GetRenderer().CreateBlendState( { renderer::BlendOperand::BLEND_SRC_ALPHA, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_INV_SRC_ALPHA },
		{ renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ZERO } );

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

			if( ImGui::BeginTabItem( "Loaded textures" ) )
			{
				auto texturesAssets = GetEngineInstance().GetAssetsManager().GetLoadedAssetsOfType< renderer::TextureAsset >();
				for( const auto& asset : texturesAssets )
				{
					forge::imgui::DrawTexture( asset->GetPath(), *asset->GetTexture() );
				}

				ImGui::EndTabItem();
			}

			if( ImGui::BeginTabItem( "Buffers" ) )
			{
				if( dynamic_cast<renderer::DeferredRenderingPass*>( m_opaqueRenderingPass.get() ) )
				{
					forge::imgui::DrawTexture( "Normals", *static_cast<renderer::DeferredRenderingPass*>( m_opaqueRenderingPass.get() )->GetNormalsTexture() );
					forge::imgui::DrawTexture( "Diffuse", *static_cast<renderer::DeferredRenderingPass*>( m_opaqueRenderingPass.get() )->GetDiffuseTexture() );
				}

				{
					if( m_depthBufferDebugTexture == nullptr || m_depthBufferDebugTexture->GetTextureSize() != m_depthStencilBuffer->GetTexture()->GetTextureSize() )
					{
						m_depthBufferDebugTexture = m_renderer->CreateTexture( GetEngineInstance().GetWindow().GetWidth(), GetEngineInstance().GetWindow().GetHeight(),
							renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE,
							renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );
					}

					const renderer::ICamera& currentCamera = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera();
					Float maxValue = currentCamera.GetType() == renderer::ICamera::Type::Perspective ? currentCamera.GetFarPlane() - currentCamera.GetNearPlane() : 1.0f;
					m_depthBufferDenominator = Math::Min( m_depthBufferDenominator, maxValue );
					ImGui::SliderFloat( "Denominator", &m_depthBufferDenominator, maxValue * 0.001f, maxValue );

					forge::imgui::DrawTexture( "Depth", *m_depthBufferDebugTexture );			
				}

				ImGui::EndTabItem();
			}
			else
			{
				m_depthBufferDebugTexture = nullptr;
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void systems::RenderingSystem::OnBeforeDrawDebug()
{
	if( m_depthBufferDebugTexture )
	{
		struct CB
		{
			Float Denominator = 1.0f;
			Float padding[ 3 ];
		};

		auto cb = m_renderer->CreateStaticConstantBuffer< CB >();
		cb->GetData().Denominator = m_depthBufferDenominator;
		cb->UpdateBuffer();
		cb->SetPS( renderer::PSConstantBufferType::Material );

		const renderer::ICamera& currentCamera = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera();
		renderer::FullScreenRenderingPass fsPass( *m_renderer, "DepthBufferDebug.fx", currentCamera.HasNonLinearDepth() ? forge::ArraySpan< renderer::ShaderDefine >{ renderer::ShaderDefine{ "__NON_LINEAR_DEPTH__" } } : forge::ArraySpan< renderer::ShaderDefine >{} );
		fsPass.SetTargetTexture( *m_depthBufferDebugTexture );
		fsPass.Draw( { m_depthStencilBuffer->GetTexture()->GetShaderResourceView() } );
	}
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
#ifdef FORGE_IMGUI_ENABLED
	OnBeforeDrawDebug();
#endif

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

	{
		PC_SCOPE( "RenderingSystem::OnDraw::CollectingRenderables" );
		for( systems::Archetype* archetype : archetypes )
		{
			const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
			const forge::DataPackage< forge::RenderingComponentData >& renderableComponents = archetype->GetData< forge::RenderingComponentData >();

			for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
			{
				renderables.emplace_back( renderableComponents[ i ].m_renderable );
			}
		}
	}

	m_rawRenderablesPacks = m_renderer->CreateRawRenderablesPackage( renderables );

	{
		PC_SCOPE( "RenderingSystem::OnDraw::UpdatingBuffers" );
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
	}

	auto& lightingSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::LightingSystem >();
	renderer::LightingData lightingData = lightingSystem.GetLightingData();
	m_shadowMapsGenerator->GenerateShadowMaps( *m_rawRenderablesPacks, lightingData );

	m_opaqueRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), m_rawRenderablesPacks->GetRendenderablesPack( renderer::RenderingPass::Opaque ), &lightingData );
	m_overlayRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), m_rawRenderablesPacks->GetRendenderablesPack( renderer::RenderingPass::Overlay ), nullptr );

	m_transparencyBlendState->Set();
	m_depthStencilState->EnableWrite( false );
	m_transparentRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), m_rawRenderablesPacks->GetRendenderablesPack( renderer::RenderingPass::Transparent ), nullptr );
	m_depthStencilState->EnableWrite( true );
	m_transparencyBlendState->Clear();

	m_renderer->SetViewportSize( Vector2( static_cast< Float >( GetEngineInstance().GetWindow().GetWidth() ), static_cast<Float>( GetEngineInstance().GetWindow().GetHeight() ) ) );
	renderer::FullScreenRenderingPass copyResourcePass( *m_renderer, "CopyTexture.fx", {} );
	copyResourcePass.SetTargetTexture( m_renderer->GetSwapchain()->GetBackBuffer() );
	copyResourcePass.Draw( { m_targetTexture->GetShaderResourceView() } );
}

void systems::RenderingSystem::OnPresent()
{
	m_renderer->GetSwapchain()->Present();
}
