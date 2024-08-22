#include "Fpch.h"
#include "LightingSystem.h"
#include "../Renderer/IRenderingPass.h"
#include "SceneRenderingSystem.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../renderer/IDepthStencilBuffer.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../IMGUI/PublicDefaults.h"
#include "../Renderer/FullscreenRenderingPass.h"
#include "../Core/IWindow.h"
#include "../Renderer/IShader.h"
#endif

#include "DebugSystem.h"
#include "../ECS/Query.h"
#include "../GameEngine/RenderingManager.h"

RTTI_IMPLEMENT_TYPE( systems::LightingSystem );

const Float c_shadowMapBaseSize = 1024.0f;

void systems::LightingSystem::OnInitialize()
{
#ifdef FORGE_IMGUI_ENABLED
	InitializeDebuggable< systems::LightingSystem >( GetEngineInstance() );
#endif

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostUpdate, [ this ]() { Update(); } );
}

renderer::LightingData systems::LightingSystem::GetLightingData()
{
	return { GetAmbientColor(), GetPointLights(), GetSpotLights(), GetDirectionalLights() };
}

void systems::LightingSystem::Update()
{
	Uint32 shadowMapsResolution = static_cast< Uint32 >( c_shadowMapBaseSize * m_shadowsResolutionScale );

	if( m_shadowMapsRecreationForced )
	{
		m_pointsLightsData.clear();
		m_spotLightsData.clear();
		m_directionalLightsData.clear();
		m_shadowMapsRecreationForced = false;
	}

	{
		ecs::Query query( GetEngineInstance().GetECSManager() );
		query.AddFragmentRequirement< forge::PointLightFragment >( ecs::Query::RequirementType::Included );
		query.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );

		Uint32 lightsAmount = 0u;
		query.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
		{
			lightsAmount += archetype.GetEntitiesAmount();
		} );

		m_pointsLightsData.resize( Math::Min( static_cast< Uint32 >( m_pointsLightsData.size() ), lightsAmount ) );
		for( Uint32 i = static_cast< Uint32 >( m_pointsLightsData.size() ); i < lightsAmount; ++i )
		{
			auto shadowMap = GetEngineInstance().GetRenderingManager().GetRenderer().CreateDepthStencilBuffer( shadowMapsResolution, shadowMapsResolution, true );
			m_pointsLightsData.push_back( { renderer::PointLightData(), std::move( shadowMap ) } );
		}

		Uint32 lightOffset = 0u;
		query.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
		{
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
			auto lightFragments = archetype.GetFragments< forge::PointLightFragment >();
			for( Uint32 i = 0; i < archetype.GetEntitiesAmount(); ++i, ++lightOffset )
			{
				renderer::PerspectiveCamera camera( 1.0f, FORGE_PI_HALF, 1.0f, 1000.0f);
				camera.SetPosition( transformFragments[ i ].m_transform.GetPosition3() );
				m_pointsLightsData[ lightOffset ].m_shaderData = renderer::PointLightData{ camera.GetProjectionMatrix(), transformFragments[ i ].m_transform.GetPosition3(), lightFragments[ i ].m_power, lightFragments[ i ].m_color };
			}
		} );
	}

	{
		ecs::Query query( GetEngineInstance().GetECSManager() );
		query.AddFragmentRequirement< forge::SpotLightFragment >( ecs::Query::RequirementType::Included );
		query.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );

		Uint32 lightsAmount = 0u;
		query.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
		{
			lightsAmount += archetype.GetEntitiesAmount();
		} );

		m_spotLightsData.resize( Math::Min( static_cast< Uint32 >( m_spotLightsData.size() ), lightsAmount ) );
		for( Uint32 i = static_cast< Uint32 >( m_spotLightsData.size() ); i < lightsAmount; ++i )
		{
			auto shadowMap = GetEngineInstance().GetRenderingManager().GetRenderer().CreateDepthStencilBuffer( shadowMapsResolution, shadowMapsResolution, false );
			m_spotLightsData.push_back( { renderer::SpotLightData(), std::move( shadowMap ) } );
		}

		Uint32 lightOffset = 0u;
		query.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
		{
			auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
			auto lightFragments = archetype.GetFragments< forge::SpotLightFragment >();
			for( Uint32 i = 0; i < archetype.GetEntitiesAmount(); ++i, ++lightOffset )
			{
				auto camera = renderer::PerspectiveCamera( 1.0f, lightFragments[ i ].m_outerAngle * 2.0f, 1.0f, 1000.0f );
				camera.SetPosition( transformFragments[ i ].m_transform.GetPosition3() );
				camera.SetOrientation( Quaternion::CreateFromDirection( transformFragments[ i ].m_transform.GetForward() ) );

				m_spotLightsData[ lightOffset ].m_shaderData = renderer::SpotLightData{ transformFragments[ i ].m_transform.GetPosition3(), transformFragments[ i ].m_transform.GetForward(),
					lightFragments[ i ].m_innerAngle, lightFragments[ i ].m_outerAngle, lightFragments[ i ].m_power, lightFragments[ i ].m_color, camera.GetViewProjectionMatrix() };
			}
		} );
	}

	{
		ecs::Query query( GetEngineInstance().GetECSManager() );
		query.AddFragmentRequirement< forge::DirectionalLightFragment >( ecs::Query::RequirementType::Included );

		Uint32 lightsAmount = 0u;
		query.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
		{
			lightsAmount += archetype.GetEntitiesAmount();
		} );

		m_directionalLightsData.resize( Math::Min( static_cast< Uint32 >( m_directionalLightsData.size() ), lightsAmount ) );
		for( Uint32 i = static_cast< Uint32 >( m_directionalLightsData.size() ); i < lightsAmount; ++i )
		{
			auto shadowMap = GetEngineInstance().GetRenderingManager().GetRenderer().CreateDepthStencilBuffer( shadowMapsResolution, shadowMapsResolution );
			m_directionalLightsData.push_back( { renderer::DirectionalLightData(), std::move( shadowMap ) } );
		}

		Uint32 lightOffset = 0u;
		query.VisitArchetypes( [ & ]( ecs::ArchetypeView archetype )
		{
			auto lightFragments = archetype.GetFragments< forge::DirectionalLightFragment >();
			for( Uint32 i = 0; i < archetype.GetEntitiesAmount(); ++i, ++lightOffset )
			{
				m_directionalLightsData[ lightOffset ].m_shaderData = { lightFragments[ i ].Direction, 0.0f, lightFragments[ i ].Color, 0.0f, lightFragments[ i ].VP };
			}
		} );
	}
}

#ifdef FORGE_IMGUI_ENABLED
void systems::LightingSystem::OnRenderDebug()
{
	m_temporaryTextures.clear();

	auto castShadowFunc = [ & ]( auto& light, Bool isPointLight )
	{
		Bool castShadows = light.m_shadowMap != nullptr;
		if( ImGui::Checkbox( "Cast Shadows", &castShadows ) )
		{
			if( castShadows )
			{
				Uint32 shadowMapsResolution = static_cast<Uint32>( c_shadowMapBaseSize * m_shadowsResolutionScale );
				light.m_shadowMap = GetEngineInstance().GetRenderingManager().GetRenderer().CreateDepthStencilBuffer( shadowMapsResolution, shadowMapsResolution, isPointLight );
			}
			else
			{
				light.m_shadowMap = nullptr;
			}
		}
	};

	auto drawTextureFunc = [ & ]( const renderer::ITexture& texture, forge::ArraySpan< renderer::ShaderDefine > shaderDefines )
	{
		Vector2 shadowMapSize = texture.GetSize();
		auto tempTexture = GetEngineInstance().GetRenderingManager().GetRenderer().CreateTexture( static_cast<Uint32>( shadowMapSize.X ), static_cast<Uint32>( shadowMapSize.Y ),
			renderer::ITexture::Flags::BIND_RENDER_TARGET | renderer::ITexture::Flags::BIND_SHADER_RESOURCE,
			renderer::ITexture::Format::R8G8B8A8_UNORM, renderer::ITexture::Type::Texture2D, renderer::ITexture::Format::R8G8B8A8_UNORM );


		forge::imgui::DrawFoldableTextureView( "Depth", *tempTexture, false, [ & ]()
		{
			struct CB
			{
				Float Denominator = 1.0f;
				Float padding[ 3 ];
			};

			auto cb = GetEngineInstance().GetRenderingManager().GetRenderer().CreateStaticConstantBuffer< CB >();
			cb->GetData().Denominator = m_depthBufferDenominator;
			cb->UpdateBuffer();
			cb->SetPS( renderer::PSConstantBufferType::Material );

			renderer::FullScreenRenderingPass fsPass( GetEngineInstance().GetRenderingManager().GetRenderer(), forge::Path( "DepthBufferDebug.fx" ), shaderDefines );
			fsPass.SetTargetTexture( *tempTexture );
			fsPass.Draw( { texture.GetShaderResourceView() } );

			const Float maxValue = 10000.0f;
			m_depthBufferDenominator = Math::Min( m_depthBufferDenominator, maxValue );
			ImGui::SliderFloat( "Denominator", &m_depthBufferDenominator, maxValue * 0.0001f, maxValue );
		} );

		m_temporaryTextures.emplace_back( std::move( tempTexture ) );
	};

	if( ImGui::Begin( "Lighting System" ) )
	{
		{
			Int32 scale = static_cast<Int32>( m_shadowsResolutionScale * 100.0f );
			if( ImGui::SliderInt( "##ResolutionScale", &scale, 1, 1600, "Resolution Scale: %d%%" ) )
			{
				m_shadowsResolutionScale = static_cast<Float>( scale ) * 0.01f;
				ForceShadowMapsRecreation();
			}
		}

		{
			ImGui::ColorEdit3( "Ambient Color", m_ambientColor.AsArray(), ImGuiColorEditFlags_NoInputs );
		}

		if( ImGui::TreeNodeEx( "Point Lights", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			ecs::Query query( GetEngineInstance().GetECSManager() );
			query.AddFragmentRequirement< forge::PointLightFragment >( ecs::Query::RequirementType::Included );
			query.AddMutableFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );

			query.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype )
			{
				auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
				auto lightFragments = archetype.GetMutableFragments< forge::PointLightFragment >();
				for( Uint32 i = 0; i < archetype.GetEntitiesAmount(); ++i )
				{
					if( ImGui::TreeNodeEx( std::to_string( i ).c_str(), ImGuiTreeNodeFlags_DefaultOpen) )
					{
						ImGui::ColorEdit3( "Color", lightFragments[i].m_color.AsArray(), ImGuiColorEditFlags_NoInputs );
						ImGui::SliderFloat( "Power", &lightFragments[i].m_power, 0.0f, 10.0f );

						GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawSphere( transformFragments[ i ].m_transform.GetPosition3(), 1.0f, lightFragments[ i ].m_color, true, false, 0.0f );

						castShadowFunc( GetPointLights()[ i ], true );

						for( Uint32 c = 0; c < 6u; ++c )
						{
							if( GetPointLights()[ i ].m_shadowMap )
							{
								drawTextureFunc( *GetPointLights()[ i ].m_shadowMap->GetTexture(), { renderer::ShaderDefine{ "__NON_LINEAR_DEPTH__" }, renderer::ShaderDefine{ "__DEPTH_CUBE_INDEX__", std::to_string( c ) } } );
							}
						}

						ImGui::TreePop();
					}
				}
			} );
			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Spot Lights", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			ecs::Query query( GetEngineInstance().GetECSManager() );
			query.AddMutableFragmentRequirement< forge::SpotLightFragment >( ecs::Query::RequirementType::Included );
			query.AddFragmentRequirement< forge::TransformFragment >( ecs::Query::RequirementType::Included );

			query.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype )
			{
				auto transformFragments = archetype.GetFragments< forge::TransformFragment >();
				auto lightFragments = archetype.GetMutableFragments< forge::SpotLightFragment >();
				for( Uint32 i = 0; i < archetype.GetEntitiesAmount(); ++i )
				{
					if( ImGui::TreeNodeEx( std::to_string( i ).c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
					{
						ImGui::ColorEdit3( "Color", lightFragments[ i ].m_color.AsArray(), ImGuiColorEditFlags_NoInputs );
						ImGui::SliderFloat( "Power", &lightFragments[ i ].m_power, 0.0f, 10.0f );
						ImGui::SliderFloat( "Inner Angle", &lightFragments[ i ].m_innerAngle, 0.0f, lightFragments[ i ].m_outerAngle - 0.01f );
						ImGui::SliderFloat( "Outer Angle", &lightFragments[ i ].m_outerAngle, 0.0f, FORGE_PI_HALF );

						Vector3 pos = transformFragments[ i ].m_transform.GetPosition3();
						Vector3 forward = transformFragments[ i ].m_transform.GetForward();

						GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawCone( pos, pos + forward * 10.0f, lightFragments[ i ].m_innerAngle, lightFragments[ i ].m_color, true, false, 0.0f );
						GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawCone( pos, pos + forward * 10.0f, lightFragments[ i ].m_outerAngle, lightFragments[ i ].m_color, true, false, 0.0f );

						castShadowFunc( GetSpotLights()[ i ], false );

						if( GetSpotLights()[ i ].m_shadowMap )
						{
							drawTextureFunc( *GetSpotLights()[ i ].m_shadowMap->GetTexture(), { renderer::ShaderDefine{ "__NON_LINEAR_DEPTH__" } } );
						}

						ImGui::TreePop();
					}
				}
			} );
			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Directional Lights", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			ecs::Query query( GetEngineInstance().GetECSManager() );
			query.AddMutableFragmentRequirement< forge::DirectionalLightFragment >( ecs::Query::RequirementType::Included );

			Uint32 lightsAmount = 0u;
			query.VisitArchetypes( [ & ]( ecs::MutableArchetypeView archetype )
			{
				auto lightFragments = archetype.GetMutableFragments< forge::DirectionalLightFragment >();
				for( Uint32 i = 0; i < archetype.GetEntitiesAmount(); ++i )
				{
					if( ImGui::TreeNodeEx( std::to_string( i ).c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
					{
						ImGui::ColorEdit3( "Color", lightFragments[ i ].Color.AsArray(), ImGuiColorEditFlags_NoInputs );

						castShadowFunc( GetDirectionalLights()[ i ], false );

						if( GetDirectionalLights()[ i ].m_shadowMap )
						{
							drawTextureFunc( *GetDirectionalLights()[ i ].m_shadowMap->GetTexture(), {} );
						}

						ImGui::TreePop();
					}
				}
			} );
			ImGui::TreePop();
		}
	}
	ImGui::End();
}
#endif