#include "Fpch.h"
#include "LightingSystem.h"
#include "../Renderer/IRenderingPass.h"

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
	{
		m_pointsLightsData.clear();
		for( auto archetype : GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( PointLightArchetypeType() ) )
		{
			const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
			const forge::DataPackage< forge::PointLightComponentData >& lightsData = archetype->GetData< forge::PointLightComponentData >();

			for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
			{
				m_pointsLightsData.emplace_back( transformComponents[ i ].m_transform.GetPosition3(), lightsData[ i ].m_power, lightsData[ i ].m_color );
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
				m_spotLightsData.emplace_back( transformComponents[ i ].m_transform.GetPosition3(), transformComponents[ i ].m_transform.GetForward(),
					lightsData[ i ].m_innerAngle, lightsData[ i ].m_outerAngle, lightsData[ i ].m_power, lightsData[ i ].m_color );
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
				m_directionalLightsData.emplace_back( light );
			}
		}
	}
}

#ifdef FORGE_IMGUI_ENABLED

void systems::LightingSystem::OnRenderDebug()
{
	{
		const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( PointLightArchetypeType() );
		for( systems::Archetype* archetype : archetypes )
		{
			forge::DataPackage< forge::TransformComponentData >& transforms = archetype->GetData< forge::TransformComponentData >();
			const forge::DataPackage< forge::PointLightComponentData >& lightsData = archetype->GetData< forge::PointLightComponentData >();
			for( Uint32 i = 0; i < transforms.GetDataSize(); ++i )
			{
				GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawSphere( transforms[ i ].m_transform.GetPosition3(), 50.0f, lightsData[ i ].m_color, 0.0f );
			}
		}
	}

	{
		const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesWithDataTypes( SpotLightArchetypeType() );
		for( systems::Archetype* archetype : archetypes )
		{
			forge::DataPackage< forge::TransformComponentData >& transforms = archetype->GetData< forge::TransformComponentData >();
			const forge::DataPackage< forge::SpotLightComponentData >& lightsData = archetype->GetData< forge::SpotLightComponentData >();
			for( Uint32 i = 0; i < transforms.GetDataSize(); ++i )
			{
				Float size = 20.0f;
				Float extent = 30.0f;
				Vector3 forward = transforms[ i ].m_transform.GetForward();
				Vector3 forwardAbs = Vector3( Math::Abs( forward.X ), Math::Abs( forward.Y ), Math::Abs( forward.Z ) );

				Vector3 pos = transforms[ i ].m_transform.GetPosition3() + forward * ( extent + size ) * 0.5f;
				Vector3 extents = Vector3::ONES() * size + forwardAbs * extent;

				GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawSphere( transforms[ i ].m_transform.GetPosition3(), 50.0f, lightsData[ i ].m_color, 0.0f );
				GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawCube( pos, extents, lightsData[ i ].m_color, 0.0f );
			}
		}
	}
}
#endif