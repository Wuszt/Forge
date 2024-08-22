#include "Fpch.h"
#include "../Renderer/PublicDefaults.h"
#include "SceneRenderingSystem.h"

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
#include "../Renderer/TextureAsset.h"
#include "../Core/AssetsManager.h"
#include "../ECS/Query.h"
#include "../Renderer/SkyboxRenderingPass.h"
#include "../GameEngine/RenderingManager.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../IMGUI/PublicDefaults.h"
#include "../Renderer/ICamera.h"
#include "../IMGUI/IMGUIMenuBar.h"
#endif

RTTI_IMPLEMENT_TYPE( systems::SceneRenderingSystem );

systems::SceneRenderingSystem::SceneRenderingSystem() = default;
systems::SceneRenderingSystem::SceneRenderingSystem( SceneRenderingSystem&& ) = default;
systems::SceneRenderingSystem::~SceneRenderingSystem() = default;

void systems::SceneRenderingSystem::OnPostInit()
{
#ifdef FORGE_IMGUI_ENABLED
	InitializeDebuggable< systems::SceneRenderingSystem >( GetEngineInstance() );
#endif

	m_renderer = &GetEngineInstance().GetRenderingManager().GetRenderer();
	m_camerasSystem = &GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >();

#ifdef FORGE_IMGUI_ENABLED
	m_menuBarButton = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().GetMenuBar().AddButton( { "Reload shaders" }, [ this ]()
		{
			GetEngineInstance().GetRenderingManager().GetRenderer().GetShadersManager()->ClearCache();
		}, false );

	m_overlayDebugToken = GetEngineInstance().GetSystemsManager().GetSystem< systems::IMGUISystem >().AddOverlayListener( [ this ]()
	{
		ImGui::Text( "Window res: %u x %u", GetEngineInstance().GetRenderingManager().GetWindow().GetWidth(), GetEngineInstance().GetRenderingManager().GetWindow().GetHeight() );

		if( m_targetTexture )
		{
			const Vector2 renderingRes = GetRenderingResolution();
			ImGui::Text( "Rendering res(%u%%) : %u x %u", static_cast<Uint32>( m_renderingResolutionScale * 100.0f ), static_cast< Uint32 >( renderingRes.X ), static_cast< Uint32 >( renderingRes.Y ) );
		}
	} );
#endif
}

#ifdef FORGE_IMGUI_ENABLED
void systems::SceneRenderingSystem::OnRenderDebug()
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
					ecs::Query renderablesQuery( GetEngineInstance().GetECSManager() );
					renderablesQuery.AddFragmentRequirement< forge::RenderableFragment >( ecs::Query::RequirementType::Included );
					renderablesQuery.VisitArchetypes( [ this ]( ecs::ArchetypeView archetype, ecs::Query::DelayedCommands& cmds )
						{
							for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
							{
								cmds.AddCommand( [ this, entityID = archetype.GetEntityIDWithIndex( i ) ]()
									{
										if ( m_debugForceWireFrame )
										{
											if ( !GetEngineInstance().GetECSManager().GetEntityArchetypeId( entityID ).ContainsTag< renderer::WireFrameTag >() )
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
					forge::imgui::DrawFoldableTextureView( asset->GetPath().AsString(), *asset->GetTexture());
				}

				ImGui::EndTabItem();
			}

			if( ImGui::BeginTabItem( "Buffers" ) )
			{
				if( dynamic_cast<renderer::DeferredRenderingPass*>( m_opaqueRenderingPass.get() ) )
				{
					forge::imgui::DrawFoldableTextureView( "Normals", *static_cast<renderer::DeferredRenderingPass*>( m_opaqueRenderingPass.get() )->GetNormalsTexture() );
					forge::imgui::DrawFoldableTextureView( "Diffuse", *static_cast<renderer::DeferredRenderingPass*>( m_opaqueRenderingPass.get() )->GetDiffuseTexture() );
				}

				{
					if( m_depthBufferDebugTexture == nullptr || m_depthBufferDebugTexture->GetSize() != m_depthStencilBuffer->GetTexture()->GetSize() )
					{
						m_depthBufferDebugTexture = m_renderer->CreateTexture( GetEngineInstance().GetRenderingManager().GetWindow().GetWidth(), GetEngineInstance().GetRenderingManager().GetWindow().GetHeight(),
							renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE,
							renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );
					}

					const renderer::ICamera& currentCamera = GetEngineInstance().GetSystemsManager().GetSystem< systems::CamerasSystem >().GetActiveCamera()->GetCamera();
					Float maxValue = currentCamera.GetCameraType() == renderer::ICamera::CameraType::Perspective ? currentCamera.GetFarPlane() - currentCamera.GetNearPlane() : 1.0f;
					m_depthBufferDenominator = Math::Min( m_depthBufferDenominator, maxValue );
					ImGui::SliderFloat( "Denominator", &m_depthBufferDenominator, maxValue * 0.001f, maxValue );

					forge::imgui::DrawFoldableTextureView( "Depth", *m_depthBufferDebugTexture );			
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

void systems::SceneRenderingSystem::CacheDepthBufferForDebug()
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
		renderer::FullScreenRenderingPass fsPass( *m_renderer, forge::Path( "DepthBufferDebug.fx" ), currentCamera.HasNonLinearDepth() ? forge::ArraySpan< renderer::ShaderDefine >{ renderer::ShaderDefine{ "__NON_LINEAR_DEPTH__" } } : forge::ArraySpan< renderer::ShaderDefine >{} );
		fsPass.SetTargetTexture( *m_depthBufferDebugTexture );
		fsPass.Draw( { m_depthStencilBuffer->GetTexture()->GetShaderResourceView() } );
	}
}
#endif

void systems::SceneRenderingSystem::Initialize( renderer::ITexture& targetTexture )
{
	renderer::ITexture* previousTargetTexture = m_targetTexture;
	m_targetTexture = &targetTexture;

	m_onTargetTextureResize = m_targetTexture->GetOnResizedCallback().AddListener(
		[ & ]( const Vector2& newSize )
		{
			UpdateRenderingResolution( m_renderingResolutionScale );
		} );

	if ( previousTargetTexture )
	{
		UpdateRenderingResolution( m_renderingResolutionScale );
		return;
	}

	const Vector2 renderingResolution = GetRenderingResolution();;
	const Uint32 x = static_cast< Uint32 >( renderingResolution.X );
	const Uint32 y = static_cast< Uint32 >( renderingResolution.Y );

	m_depthStencilBuffer = m_renderer->CreateDepthStencilBuffer( x, y );
	m_shadowMapsGenerator = std::make_unique<renderer::ShadowMapsGenerator>( *m_renderer );

	{
		m_samplerStates.emplace_back( m_renderer->CreateSamplerState( renderer::SamplerStateFilterType::MIN_MAG_MIP_LINEAR, renderer::SamplerStateComparisonType::ALWAYS ) );
		m_samplerStates.emplace_back( m_renderer->CreateSamplerState( renderer::SamplerStateFilterType::COMPARISON_MIN_MAG_LINEAR_MIP_POINT, renderer::SamplerStateComparisonType::LESS ) );

		std::vector< renderer::ISamplerState* > samplerStates;
		for ( auto& samplerState : m_samplerStates )
		{
			samplerStates.emplace_back( samplerState.get() );
		}
	}

	m_beforeDrawToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PreRendering, std::bind( &systems::SceneRenderingSystem::OnBeforeDraw, this ) );
	m_drawToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Rendering, std::bind( &systems::SceneRenderingSystem::OnDraw, this ) );

	m_intermediateTexture = m_renderer->CreateTexture( x, y,
		renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE,
		renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );

	if( m_skyboxRenderingPass )
	{
		m_skyboxRenderingPass->SetTargetTexture( *m_intermediateTexture );
	}

	SetRenderingMode( RenderingMode::Deferred );

	m_transparentRenderingPass = std::make_unique< renderer::ForwardRenderingPass >( *m_renderer );
	m_transparentRenderingPass->SetTargetTexture( *m_intermediateTexture );
	m_transparentRenderingPass->SetDepthStencilBuffer( m_depthStencilBuffer.get() );

	m_transparencyBlendState = GetEngineInstance().GetRenderingManager().GetRenderer().CreateBlendState( { renderer::BlendOperand::BLEND_SRC_ALPHA, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_INV_SRC_ALPHA },
		{ renderer::BlendOperand::BLEND_ONE, renderer::BlendOperation::BLEND_OP_ADD, renderer::BlendOperand::BLEND_ZERO } );

	std::vector< renderer::ShaderDefine > baseShaderDefines;
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::DeferredRenderingPass::GetRequiredShaderDefines().begin(), renderer::DeferredRenderingPass::GetRequiredShaderDefines().end() );
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::ForwardRenderingPass::GetRequiredShaderDefines().begin(), renderer::ForwardRenderingPass::GetRequiredShaderDefines().end() );
	baseShaderDefines.insert( baseShaderDefines.end(), renderer::ShadowsRenderingPass::GetRequiredShaderDefines().begin(), renderer::ShadowsRenderingPass::GetRequiredShaderDefines().end() );
	m_renderer->GetShadersManager()->SetBaseShaderDefines( std::move( baseShaderDefines ) );

	m_depthStencilState = m_renderer->CreateDepthStencilState( renderer::DepthStencilComparisonFunc::COMPARISON_LESS_EQUAL );
	m_depthStencilState->Set();
}

void systems::SceneRenderingSystem::Deinitialize()
{
	m_onTargetTextureResize.Unregister();
	m_targetTexture = nullptr;
	m_depthStencilBuffer = nullptr;
	m_shadowMapsGenerator = nullptr;
	m_beforeDrawToken.Unregister();
	m_drawToken.Unregister();
	m_intermediateTexture = nullptr;
	m_transparentRenderingPass = nullptr;
	m_transparencyBlendState = nullptr;
	m_depthStencilState = nullptr;
}

void systems::SceneRenderingSystem::SetRenderingMode( RenderingMode renderingMode )
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
	m_opaqueRenderingPass->SetTargetTexture( *m_intermediateTexture );
	m_opaqueRenderingPass->SetDepthStencilBuffer( m_depthStencilBuffer.get() );
}

void systems::SceneRenderingSystem::UpdateRenderingResolution( Float scale )
{
	m_renderingResolutionScale = scale;
	const Vector2 renderingResolution = GetRenderingResolution();

	const Uint32 renderingResolutionWidth = static_cast<Uint32>( renderingResolution.X );
	const Uint32 renderingResolutionHeight = static_cast<Uint32>( renderingResolution.Y );
	m_depthStencilBuffer->Resize( renderingResolutionWidth, renderingResolutionHeight );
	m_intermediateTexture->Resize( renderingResolution );
	m_onChangedRenderingResolution.Invoke( renderingResolution );
}

Vector2 systems::SceneRenderingSystem::GetRenderingResolution()
{
	Vector2 result = m_targetTexture->GetSize();
	result *= m_renderingResolutionScale;
	result.X = static_cast< Float >( static_cast< Uint32 >( result.X ) );
	result.Y = static_cast< Float >( static_cast< Uint32 >( result.Y ) );

	return result;
}

void systems::SceneRenderingSystem::SetSkyboxTexture( std::shared_ptr< const renderer::ITexture > texture )
{
	m_skyboxRenderingPass = nullptr;
	if ( texture )
	{
		m_skyboxRenderingPass = std::make_unique<renderer::SkyboxRenderingPass>( GetEngineInstance().GetAssetsManager(), GetEngineInstance().GetRenderingManager().GetRenderer(), texture );

		if ( m_intermediateTexture )
		{
			m_skyboxRenderingPass->SetTargetTexture( *m_intermediateTexture );
		}
	}
}

void systems::SceneRenderingSystem::SetTargetTexture( renderer::ITexture* texture )
{
	if ( texture )
	{
		Initialize( *texture );
	}
	else
	{
		Deinitialize();
	}
}

namespace
{
	struct ContainsTransparentShapes : public ecs::Tag
	{
		RTTI_DECLARE_STRUCT( ContainsTransparentShapes, ecs::Tag );
	};

	RTTI_IMPLEMENT_TYPE( ContainsTransparentShapes );
}

void systems::SceneRenderingSystem::OnBeforeDraw()
{
	m_opaqueRenderingPass->ClearTargetTexture(); // this is fucked up, what about other rendering passes?

	{
		PC_SCOPE( "SceneRenderingSystem::UpdatingRawRenderables" );
		ecs::Query renderablesToUpdate( GetEngineInstance().GetECSManager() );
		renderablesToUpdate.AddTagRequirement< forge::DirtyRenderable >( ecs::Query::RequirementType::Included );
		renderablesToUpdate.AddFragmentRequirement< forge::RenderableFragment >( ecs::Query::RequirementType::Included );
		renderablesToUpdate.AddMutableFragmentRequirement( m_renderer->GetECSFragmentType(), ecs::Query::RequirementType::Included );

		renderablesToUpdate.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype, ecs::Query::DelayedCommands& cmds )
		{
			auto renderables = archetype.GetFragments< forge::RenderableFragment >();
			m_renderer->UpdateRenderableECSArchetype( archetype, [&renderables]( Uint32 index ) -> const renderer::Renderable& { return renderables[ index ].m_renderable; } );

			for ( Uint32 i = 0u; i < archetype.GetEntitiesAmount(); ++i )
			{
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
				else if ( GetEngineInstance().GetECSManager().GetEntityArchetypeId( entityID ).ContainsTag< renderer::WireFrameTag >() )
				{
					cmds.AddCommand( [ this, entityID ]()
						{
							GetEngineInstance().GetECSManager().RemoveTagFromEntity< renderer::WireFrameTag >( entityID );
						} );
				}

				cmds.AddCommand( [ this, entityID ]()
				{
					{
						Bool containsTransparentMaterials = false;
						{
							auto renderableFragment = GetEngineInstance().GetECSManager().GetFragmentView< forge::RenderableFragment >( entityID );

							for ( auto& material : renderableFragment->m_renderable.GetMaterials() )
							{
								containsTransparentMaterials |= material->GetRenderingPass() == renderer::RenderingPass::Transparent;
							}
						}

						if ( containsTransparentMaterials )
						{
							if ( !GetEngineInstance().GetECSManager().GetEntityArchetypeId( entityID ).ContainsTag< ContainsTransparentShapes >() )
							{
								GetEngineInstance().GetECSManager().AddTagToEntity< ContainsTransparentShapes >( entityID );
							}
						}
						else
						{
							if ( GetEngineInstance().GetECSManager().GetEntityArchetypeId( entityID ).ContainsTag< ContainsTransparentShapes >() )
							{
								GetEngineInstance().GetECSManager().RemoveTagFromEntity< ContainsTransparentShapes >( entityID );
							}
						}
					}

					GetEngineInstance().GetECSManager().RemoveTagFromEntity< forge::DirtyRenderable >( entityID );
				} );
			}
		} );
	}

	ecs::Query modifiedTransformQuery( GetEngineInstance().GetECSManager() );
	modifiedTransformQuery.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );
	modifiedTransformQuery.AddMutableFragmentRequirement< forge::RenderableFragment >( ecs::Query::RequirementType::Included );
	modifiedTransformQuery.AddTagRequirement< forge::TransformWasModifiedThisFrame >( ecs::Query::RequirementType::Included );

	{
		PC_SCOPE( "SceneRenderingSystem::OnDraw::UpdatingBuffers" );

		auto func = [ & ]( ecs::MutableArchetypeView archetype )
		{
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
			auto renderableFragments = archetype.GetMutableFragments< forge::RenderableFragment >();

			for ( Uint32 i = 0; i < archetype.GetEntitiesAmount(); ++i )
			{
				auto& cb = renderableFragments[ i ].m_renderable.GetCBMesh();
				cb.GetData().W = transformFragments[ i ].ToMatrix();
				cb.UpdateBuffer();
			}
		};

		modifiedTransformQuery.VisitArchetypes( func );
	}
}

void systems::SceneRenderingSystem::OnDraw()
{
	PC_SCOPE_FUNC();

	if ( !m_camerasSystem->GetActiveCamera() || !m_targetTexture )
	{
		return;
	}

	{
		std::vector< renderer::ISamplerState* > samplerStates;
		samplerStates.reserve( m_samplerStates.size() );
		for ( auto& samplerState : m_samplerStates )
		{
			samplerStates.emplace_back( samplerState.get() );
		}
		m_renderer->SetSamplerStates( samplerStates );
	}

	if ( m_skyboxRenderingPass )
	{
		PC_SCOPE( "SceneRenderingSystem::OnDraw::Skybox" );
		m_skyboxRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera() );
	}

	{
		PC_SCOPE( "SceneRenderingSystem::OnDraw::Opaque" );
		ecs::Query opaqueQuery( GetEngineInstance().GetECSManager() );
		opaqueQuery.AddFragmentRequirement( m_renderer->GetECSFragmentType(), ecs::Query::RequirementType::Included );
		opaqueQuery.AddTagRequirement< forge::DrawAsOverlay >( ecs::Query::RequirementType::Excluded );
		opaqueQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Excluded );

		auto& lightingSystem = GetEngineInstance().GetSystemsManager().GetSystem< systems::LightingSystem >();
		renderer::LightingData lightingData = lightingSystem.GetLightingData();

		ecs::Query shadowsQuery( GetEngineInstance().GetECSManager() );
		shadowsQuery.AddFragmentRequirement( m_renderer->GetECSFragmentType(), ecs::Query::RequirementType::Included );
		shadowsQuery.AddTagRequirement< forge::IgnoresLights >( ecs::Query::RequirementType::Excluded );
		shadowsQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Excluded );

		m_shadowMapsGenerator->GenerateShadowMaps( shadowsQuery, renderer::RenderingPass::Opaque, lightingData );
		m_opaqueRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), opaqueQuery, renderer::RenderingPass::Opaque, &lightingData );
	}

	{
		PC_SCOPE( "SceneRenderingSystem::OnDraw::Transparent" );
		ecs::Query transparentQuery( GetEngineInstance().GetECSManager() );
		transparentQuery.AddFragmentRequirement( m_renderer->GetECSFragmentType(), ecs::Query::RequirementType::Included );
		transparentQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Included );

		m_transparencyBlendState->Set();
		m_depthStencilState->EnableWrite( false );
		m_transparentRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), transparentQuery, renderer::RenderingPass::Transparent, nullptr );
		m_depthStencilState->EnableWrite( true );
		m_transparencyBlendState->Clear();
	}

	{
#ifdef FORGE_IMGUI_ENABLED
		CacheDepthBufferForDebug();
#endif

		PC_SCOPE( "SceneRenderingSystem::OnDraw::Overlay" );
		m_depthStencilBuffer->GetView().Clear();

		ecs::Query opaqueOverlayQuery( GetEngineInstance().GetECSManager() );
		opaqueOverlayQuery.AddFragmentRequirement( m_renderer->GetECSFragmentType(), ecs::Query::RequirementType::Included );
		opaqueOverlayQuery.AddTagRequirement< forge::DrawAsOverlay >( ecs::Query::RequirementType::Included );
		opaqueOverlayQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Excluded );
		m_opaqueRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), opaqueOverlayQuery, renderer::RenderingPass::Opaque, nullptr );

		ecs::Query transparentOverlayQuery( GetEngineInstance().GetECSManager() );
		transparentOverlayQuery.AddFragmentRequirement( m_renderer->GetECSFragmentType(), ecs::Query::RequirementType::Included );
		transparentOverlayQuery.AddTagRequirement< forge::DrawAsOverlay >( ecs::Query::RequirementType::Included );
		transparentOverlayQuery.AddTagRequirement< ContainsTransparentShapes >( ecs::Query::RequirementType::Included );

		m_transparencyBlendState->Set();
		m_depthStencilState->EnableWrite( false );
		m_transparentRenderingPass->Draw( m_camerasSystem->GetActiveCamera()->GetCamera(), transparentOverlayQuery, renderer::RenderingPass::Transparent, nullptr );
		m_depthStencilState->EnableWrite( true );
		m_transparencyBlendState->Clear();
	}

	renderer::FullScreenRenderingPass copyResourcePass( *m_renderer, forge::Path( "CopyTexture.fx" ), {} );
	copyResourcePass.SetTargetTexture( *m_targetTexture );
	copyResourcePass.Draw( { m_intermediateTexture->GetShaderResourceView() } );
}
