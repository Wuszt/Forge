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
#include "../Renderer/ShadowsRenderingPass.h"
#include "../Renderer/IBlendState.h"
#include "../Renderer/ShadowMapsGenerator.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../IMGUI/PublicDefaults.h"
#include "../Renderer/ICamera.h"
#endif
#include "../Renderer/TextureAsset.h"
#include "../Core/AssetsManager.h"
#include "../ECS/Query.h"
#include "../Renderer/SkyboxRenderingPass.h"

IMPLEMENT_TYPE( systems::RenderingSystem );

systems::RenderingSystem::RenderingSystem() = default;

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

	m_targetTexture = m_renderer->CreateTexture( GetEngineInstance().GetWindow().GetWidth(), GetEngineInstance().GetWindow().GetHeight(), 
		renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE,
		renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );

	SetRenderingMode( RenderingMode::Deferred );

	m_transparentRenderingPass = std::make_unique< renderer::ForwardRenderingPass >( *m_renderer );
	m_transparentRenderingPass->SetTargetTexture( *m_targetTexture );
	m_transparentRenderingPass->SetDepthStencilBuffer( m_depthStencilBuffer.get() );

	m_transparencyBlendState = GetEngineInstance().GetRenderer().CreateBlendState( { renderer::BlendOperand::BLEND_SRC_ALPHA, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_INV_SRC_ALPHA },
		{ renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ZERO } );

	std::vector< renderer::ShaderDefine > baseShaderDefines;
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::DeferredRenderingPass::GetRequiredShaderDefines().begin(), renderer::DeferredRenderingPass::GetRequiredShaderDefines().end() );
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::ForwardRenderingPass::GetRequiredShaderDefines().begin(), renderer::ForwardRenderingPass::GetRequiredShaderDefines().end() );
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::ShadowsRenderingPass::GetRequiredShaderDefines().begin(), renderer::ShadowsRenderingPass::GetRequiredShaderDefines().end() );
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
	m_topBarButton = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().GetTopBar().AddButton( { "Reload shaders" }, false );
	m_topBarButtonToken = m_topBarButton->GetCallback().AddListener( [ this ]()
		{
			GetEngineInstance().GetRenderer().GetShadersManager()->ClearCache();
		} );

	m_overlayDebugToken = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( [ this ]()
	{
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

				if ( ImGui::Checkbox( "Wire Frame", &m_debugForceWireFrame ) )
				{
					ecs::Query renderablesQuery;
					renderablesQuery.AddFragmentRequirement< forge::RenderableFragment >( ecs::Query::RequirementType::Included );
					renderablesQuery.VisitArchetypes( GetEngineInstance().GetECSManager(), [ this ]( ecs::Archetype& archetype, ecs::Query::DelayedCommands& cmds )
						{
							for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
							{
								cmds.AddCommand( [ this, entityID = archetype.GetEntityIDWithIndex( i ) ]()
									{
										if ( m_debugForceWireFrame )
										{
											if ( !GetEngineInstance().GetECSManager().GetEntityArchetype( entityID )->GetArchetypeID().ContainsTag< renderer::WireFrameTag >() )
											{
												GetEngineInstance().GetECSManager().AddTagToEntity< renderer::WireFrameTag >( entityID );
											}								
										}
										else
										{
											GetEngineInstance().GetECSManager().AddTagToEntity< forge::DirtyRenderable >( entityID );
										}
									} );
							}
						} );
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
					Float maxValue = currentCamera.GetCameraType() == renderer::ICamera::CameraType::Perspective ? currentCamera.GetFarPlane() - currentCamera.GetNearPlane() : 1.0f;
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

void systems::RenderingSystem::SetSkyboxTexture( std::shared_ptr< const renderer::ITexture > texture )
{
	m_skyboxRenderingPass = nullptr;
	if ( texture )
	{
		m_skyboxRenderingPass = std::make_unique<renderer::SkyboxRenderingPass>( GetEngineInstance().GetAssetsManager(), GetEngineInstance().GetRenderer(), texture );
	}
}

namespace
{
	struct ContainsTransparentShapes : public ecs::Tag
	{
		DECLARE_STRUCT( ContainsTransparentShapes, ecs::Tag );
		REGISTER_ECS_TAG();
	};

	IMPLEMENT_TYPE( ContainsTransparentShapes );
}

void systems::RenderingSystem::OnBeforeDraw()
{
#ifdef FORGE_IMGUI_ENABLED
	OnBeforeDrawDebug();
#endif

	m_renderer->OnBeforeDraw();

	m_opaqueRenderingPass->ClearTargetTexture(); // this is fucked up, what about other rendering passes?

	{
		PC_SCOPE( "RenderingSystem::UpdatingRawRenderables" );
		ecs::Query renderablesToUpdate;
		renderablesToUpdate.AddTagRequirement< forge::DirtyRenderable >( ecs::Query::RequirementType::Included );
		renderablesToUpdate.AddFragmentRequirement< forge::RenderableFragment >( ecs::Query::RequirementType::Included );
		renderablesToUpdate.AddFragmentRequirement< renderer::IRawRenderableFragment >( ecs::Query::RequirementType::Included );

		renderablesToUpdate.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::Archetype& archetype, ecs::Query::DelayedCommands& cmds )
		{
		   auto renderables = archetype.GetFragments< forge::RenderableFragment >();
		   auto rawRenderables = archetype.GetFragments< renderer::IRawRenderableFragment >();

			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
				m_renderer->UpdateRenderableECSFragment( GetEngineInstance().GetECSManager(), archetype.GetEntityIDWithIndex( i ), renderables[ i ].m_renderable );

				Bool forcedWireFrame = false;

#ifdef FORGE_IMGUI_ENABLED
				forcedWireFrame = m_debugForceWireFrame;
#endif

				auto entityID = archetype.GetEntityIDWithIndex( i );

				if ( renderables[ i ].m_renderable.GetFillMode() == renderer::FillMode::WireFrame || forcedWireFrame )
				{
					cmds.AddCommand( [ this, entityID ]()
						{
							GetEngineInstance().GetECSManager().AddTagToEntity< renderer::WireFrameTag >( entityID );
						} );
				}
				else if ( GetEngineInstance().GetECSManager().GetEntityArchetype( entityID )->GetArchetypeID().ContainsTag< renderer::WireFrameTag >() )
				{
					cmds.AddCommand( [ this, entityID ]()
						{
							GetEngineInstance().GetECSManager().RemoveTagFromEntity< renderer::WireFrameTag >( entityID );
						} );
				}

				cmds.AddCommand( [ this, entityID ]()
					{
						forge::RenderableFragment* renderableFragment = GetEngineInstance().GetECSManager().GetEntityArchetype( entityID )->GetFragment< forge::RenderableFragment >( entityID );

				Bool containsTransparentMaterials = false;
				for ( auto& material : renderableFragment->m_renderable.GetMaterials() )
				{
					containsTransparentMaterials |= material->GetRenderingPass() == renderer::RenderingPass::Transparent;
				}

				if ( containsTransparentMaterials )
				{
					if ( !GetEngineInstance().GetECSManager().GetEntityArchetype( entityID )->GetArchetypeID().ContainsTag< ContainsTransparentShapes >() )
					{
						GetEngineInstance().GetECSManager().AddTagToEntity< ContainsTransparentShapes >( entityID );
					}
				}
				else
				{
					if ( GetEngineInstance().GetECSManager().GetEntityArchetype( entityID )->GetArchetypeID().ContainsTag< ContainsTransparentShapes >() )
					{
						GetEngineInstance().GetECSManager().RemoveTagFromEntity< ContainsTransparentShapes >( entityID );
					}
				}

				GetEngineInstance().GetECSManager().RemoveTagFromEntity< forge::DirtyRenderable >( entityID );
					} );
			}
		} );
	}

	ecs::Query query;
	query.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
	query.AddFragmentRequirement< renderer::IRawRenderableFragment >( ecs::Query::RequirementType::Included );
	query.AddTagRequirement< forge::TransformModifiedThisFrame >( ecs::Query::RequirementType::Included );

	{
		PC_SCOPE( "RenderingSystem::OnDraw::UpdatingBuffers" );
		query.VisitArchetypes( GetEngineInstance().GetECSManager(), [ & ]( ecs::Archetype& archetype )
		{
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
		    auto renderableFragments = archetype.GetFragments< forge::RenderableFragment >();

			for ( Uint32 i = 0; i < archetype.GetEntitiesAmount(); ++i )
			{
				auto& cb = renderableFragments[ i ].m_renderable.GetCBMesh();
				cb.GetData().W = transformFragments[ i ].ToMatrix();
				cb.UpdateBuffer();
			}
		} );
	}
}

void systems::RenderingSystem::OnDraw()
{
	PC_SCOPE_FUNC();

	if( !m_camerasSystem->GetActiveCamera() )
	{
		return;
	}

	{
		PC_SCOPE( "RenderingSystem::OnDraw::Opaque" );
		ecs::Query opaqueQuery;
		opaqueQuery.AddFragmentRequirement< renderer::IRawRenderableFragment >( ecs::Query::RequirementType::Included );
		opaqueQuery.AddTagRequirement< forge::DrawAsOverlay >( ecs::Query::RequirementType::Excluded );
		opaqueQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Excluded );

		auto& lightingSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::LightingSystem >();
		renderer::LightingData lightingData = lightingSystem.GetLightingData();

		ecs::Query shadowsQuery;
		shadowsQuery.AddFragmentRequirement< renderer::IRawRenderableFragment >( ecs::Query::RequirementType::Included );
		shadowsQuery.AddTagRequirement< forge::IgnoresLights >( ecs::Query::RequirementType::Excluded );
		shadowsQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Excluded );

		m_shadowMapsGenerator->GenerateShadowMaps( GetEngineInstance().GetECSManager(), shadowsQuery, renderer::RenderingPass::Opaque, lightingData );
		m_opaqueRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), GetEngineInstance().GetECSManager(), opaqueQuery, renderer::RenderingPass::Opaque, &lightingData );
	}

	{
		PC_SCOPE( "RenderingSystem::OnDraw::Transparent" );
		ecs::Query transparentQuery;
		transparentQuery.AddFragmentRequirement< renderer::IRawRenderableFragment >( ecs::Query::RequirementType::Included );
		transparentQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Included );

		m_transparencyBlendState->Set();
		m_depthStencilState->EnableWrite( false );
		m_transparentRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), GetEngineInstance().GetECSManager(), transparentQuery, renderer::RenderingPass::Transparent, nullptr );
		m_depthStencilState->EnableWrite( true );
		m_transparencyBlendState->Clear();
	}

	{
		PC_SCOPE( "RenderingSystem::OnDraw::Overlay" );
		m_depthStencilBuffer->GetView().Clear();

		ecs::Query opaqueOverlayQuery;
		opaqueOverlayQuery.AddFragmentRequirement< renderer::IRawRenderableFragment >( ecs::Query::RequirementType::Included );
		opaqueOverlayQuery.AddTagRequirement< forge::DrawAsOverlay >( ecs::Query::RequirementType::Included );
		opaqueOverlayQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Excluded );
		m_opaqueRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), GetEngineInstance().GetECSManager(), opaqueOverlayQuery, renderer::RenderingPass::Opaque, nullptr );

		ecs::Query transparentOverlayQuery;
		transparentOverlayQuery.AddFragmentRequirement< renderer::IRawRenderableFragment >( ecs::Query::RequirementType::Included );
		transparentOverlayQuery.AddTagRequirement< forge::DrawAsOverlay >( ecs::Query::RequirementType::Included );
		transparentOverlayQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Included );

		m_transparencyBlendState->Set();
		m_depthStencilState->EnableWrite( false );
		m_transparentRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), GetEngineInstance().GetECSManager(), transparentOverlayQuery, renderer::RenderingPass::Transparent, nullptr );
		m_depthStencilState->EnableWrite( true );
		m_transparencyBlendState->Clear();
	}

	if ( m_skyboxRenderingPass )
	{
		PC_SCOPE( "RenderingSystem::OnDraw::Skybox" );
		m_skyboxRenderingPass->SetTargetTexture( *m_targetTexture );
		m_skyboxRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera() );
	}

	m_renderer->SetViewportSize( Vector2( static_cast< Float >( GetEngineInstance().GetWindow().GetWidth() ), static_cast<Float>( GetEngineInstance().GetWindow().GetHeight() ) ) );
	renderer::FullScreenRenderingPass copyResourcePass( *m_renderer, "CopyTexture.fx", {} );
	copyResourcePass.SetTargetTexture( m_renderer->GetSwapchain()->GetBackBuffer() );
	copyResourcePass.Draw( { m_targetTexture->GetShaderResourceView() } );
}

void systems::RenderingSystem::OnPresent()
{
	m_renderer->GetSwapchain()->Present();
}
