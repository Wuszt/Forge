#include "Fpch.h"
#include "LightingSystem.h"
#include "../Renderer/IRenderingPass.h"
#include "RenderingSystem.h"
#include "EngineInstance.h"
#include "../Renderer/IRenderer.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../renderer/IDepthStencilBuffer.h"

#ifdef FORGE_IMGUI_ENABLED
#include "../../External/imgui/imgui.h"
#endif

void systems::LightingSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostUpdate, [ this ]() { Update(); } );
}

renderer::LightingData systems::LightingSystem::GetLightingData() const
{
	return { GetAmbientColor(), GetPointLights(), GetSpotLights(), GetDirectionalLights() };
}

void systems::LightingSystem::Update()
{
	const Float shadowMapSize = 1024.0f;
	Uint32 shadowMapsResolution = static_cast< Uint32 >( shadowMapSize * m_shadowsResolutionScale );

	{
		m_pointsLightsData.clear();
		for( auto archetype : GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( PointLightArchetypeType() ) )
		{
			const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
			const forge::DataPackage< forge::PointLightComponentData >& lightsData = archetype->GetData< forge::PointLightComponentData >();

			for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
			{
				auto shadowMap = GetEngineInstance().GetRenderer().CreateDepthStencilBuffer( shadowMapsResolution, shadowMapsResolution, true );

				renderer::PerspectiveCamera camera( 1.0f, FORGE_PI_HALF, 1.0f, 1000.0f);
				camera.SetPosition( transformComponents[ i ].m_transform.GetPosition3() );
				m_pointsLightsData.push_back( { renderer::PointLightData{ camera.GetProjectionMatrix(), transformComponents[ i ].m_transform.GetPosition3(), lightsData[ i ].m_power, lightsData[ i ].m_color }, std::move( shadowMap ) } );
			}
		}
	}

	{
		m_spotLightsData.clear();
		for( auto archetype : GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( SpotLightArchetypeType() ) )
		{
			const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
			const forge::DataPackage< forge::SpotLightComponentData >& lightsData = archetype->GetData< forge::SpotLightComponentData >();

			for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
			{
				auto camera = renderer::PerspectiveCamera( 1.0f, lightsData[ i ].m_outerAngle * 2.0f, 1.0f, 1000.0f );
				camera.SetPosition( transformComponents[ i ].m_transform.GetPosition3() );
				camera.SetOrientation( Quaternion::CreateFromDirection( transformComponents[ i ].m_transform.GetForward() ) );

				auto shadowMap = GetEngineInstance().GetRenderer().CreateDepthStencilBuffer( shadowMapsResolution, shadowMapsResolution );

				m_spotLightsData.push_back( { renderer::SpotLightData{ transformComponents[ i ].m_transform.GetPosition3(), transformComponents[ i ].m_transform.GetForward(),
					lightsData[ i ].m_innerAngle, lightsData[ i ].m_outerAngle, lightsData[ i ].m_power, lightsData[ i ].m_color, camera.GetViewProjectionMatrix() }, std::move( shadowMap ) } );
			}
		}
	}

	{
		m_directionalLightsData.clear();
		for( auto archetype : GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( DirectionalLightArchetypeType() ) )
		{
			const forge::DataPackage< forge::DirectionalLightComponentData >& lightsData = archetype->GetData< forge::DirectionalLightComponentData >();

			for( const auto& light : lightsData )
			{
				m_directionalLightsData.push_back( { renderer::DirectionalLightData{ light } } );
			}
		}
	}
}

#ifdef FORGE_IMGUI_ENABLED

void systems::LightingSystem::OnRenderDebug()
{
	if( ImGui::Begin( "LightingDebug" ) )
	{
		{
			Int32 scale = static_cast<Int32>( m_shadowsResolutionScale * 100.0f );
			if( ImGui::SliderInt( "##ResolutionScale", &scale, 1, 800, "Resolution Scale: %d%%" ) )
			{
				m_shadowsResolutionScale = static_cast<Float>( scale ) * 0.01f;
			}
		}

		{
			ImGui::ColorEdit3( "Ambient Color", m_ambientColor.AsArray(), ImGuiColorEditFlags_NoInputs );
		}

		if( ImGui::TreeNodeEx( "Point Lights", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( PointLightArchetypeType() );
			for( systems::Archetype* archetype : archetypes )
			{
				const forge::DataPackage< forge::TransformComponentData >& transforms = archetype->GetData< forge::TransformComponentData >();
				forge::DataPackage< forge::PointLightComponentData >& lightsData = archetype->GetData< forge::PointLightComponentData >();
				for( Uint32 i = 0; i < transforms.GetDataSize(); ++i )
				{
					if( ImGui::TreeNodeEx( std::to_string( i ).c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
					{
						ImGui::ColorEdit3( "Color", lightsData[i].m_color.AsArray(), ImGuiColorEditFlags_NoInputs );
						ImGui::SliderFloat( "Power", &lightsData[i].m_power, 0.0f, 10000.0f );

						GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawSphere( transforms[ i ].m_transform.GetPosition3(), 50.0f, lightsData[ i ].m_color, 0.0f );
						ImGui::TreePop();
					}
				}
			}
			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Spot Lights", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( SpotLightArchetypeType() );
			for( systems::Archetype* archetype : archetypes )
			{
				const forge::DataPackage< forge::TransformComponentData >& transforms = archetype->GetData< forge::TransformComponentData >();
				forge::DataPackage< forge::SpotLightComponentData >& lightsData = archetype->GetData< forge::SpotLightComponentData >();
				for( Uint32 i = 0; i < transforms.GetDataSize(); ++i )
				{
					if( ImGui::TreeNodeEx( std::to_string( i ).c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
					{
						ImGui::ColorEdit3( "Color", lightsData[ i ].m_color.AsArray(), ImGuiColorEditFlags_NoInputs );
						ImGui::SliderFloat( "Power", &lightsData[ i ].m_power, 0.0f, 10000.0f );
						ImGui::SliderFloat( "Inner Angle", &lightsData[ i ].m_innerAngle, 0.0f, lightsData[ i ].m_outerAngle - 0.01f );
						ImGui::SliderFloat( "Outer Angle", &lightsData[ i ].m_outerAngle, 0.0f, FORGE_PI_HALF );

						Float size = 20.0f;
						Float extent = 30.0f;
						Vector3 forward = transforms[ i ].m_transform.GetForward();
						Vector3 forwardAbs = Vector3( Math::Abs( forward.X ), Math::Abs( forward.Y ), Math::Abs( forward.Z ) );

						Vector3 pos = transforms[ i ].m_transform.GetPosition3() + forward * ( extent + size ) * 0.5f;
						Vector3 extents = Vector3::ONES() * size + forwardAbs * extent;

						GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawSphere( transforms[ i ].m_transform.GetPosition3(), 50.0f, lightsData[ i ].m_color, 0.0f );
						GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawCube( pos, extents, lightsData[ i ].m_color, 0.0f );
						ImGui::TreePop();
					}
				}
			}
			ImGui::TreePop();
		}

		if( ImGui::TreeNodeEx( "Directional Lights", ImGuiTreeNodeFlags_DefaultOpen ) )
		{
			const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( DirectionalLightArchetypeType() );
			for( systems::Archetype* archetype : archetypes )
			{
				const forge::DataPackage< forge::TransformComponentData >& transforms = archetype->GetData< forge::TransformComponentData >();
				forge::DataPackage< forge::DirectionalLightComponentData >& lightsData = archetype->GetData< forge::DirectionalLightComponentData >();
				for( Uint32 i = 0; i < transforms.GetDataSize(); ++i )
				{
					if( ImGui::TreeNodeEx( std::to_string( i ).c_str(), ImGuiTreeNodeFlags_DefaultOpen ) )
					{
						ImGui::ColorEdit3( "Color", lightsData[ i ].Color.AsArray(), ImGuiColorEditFlags_NoInputs );
						ImGui::TreePop();
					}
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::End();
}
#endif