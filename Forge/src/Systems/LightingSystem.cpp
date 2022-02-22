#include "Fpch.h"
#include "LightingSystem.h"

void systems::LightingSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostUpdate, [ this ]() { Update(); } );
}

forge::ArraySpan< const renderer::LightData > systems::LightingSystem::GetLights() const
{
	return m_lightsData;
}

void systems::LightingSystem::Update()
{
	m_lightsData.clear();
	for( auto archetype : GetEngineInstance().GetSystemsManager().GetArchetypesOfSystem< systems::LightingSystem >() )
	{
		const forge::DataPackage< forge::TransformComponentData >& transformComponents = archetype->GetData< forge::TransformComponentData >();
		const forge::DataPackage< forge::LightComponentData >& lightsData = archetype->GetData< forge::LightComponentData >();

		for( Uint32 i = 0; i < transformComponents.GetDataSize(); ++i )
		{
			m_lightsData.emplace_back( transformComponents[ i ].m_transform.GetPosition3(), lightsData[ i ].m_power, lightsData[ i ].m_color );
		}
	}
}

#ifdef FORGE_IMGUI_ENABLED

void systems::LightingSystem::OnRenderDebug()
{
	const auto& archetypes = GetEngineInstance().GetSystemsManager().GetArchetypesOfSystem< systems::LightingSystem >();
	for( systems::Archetype* archetype : archetypes )
	{
		forge::DataPackage< forge::TransformComponentData >& transforms = archetype->GetData< forge::TransformComponentData >();
		const forge::DataPackage< forge::LightComponentData >& lightsData = archetype->GetData< forge::LightComponentData >();
		for( Uint32 i = 0; i < transforms.GetDataSize(); ++i )
		{
			GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >().DrawSphere( transforms[ i ].m_transform.GetPosition3(), 50.0f, lightsData[ i ].m_color, 0.0f );
		}
	}
}
#endif