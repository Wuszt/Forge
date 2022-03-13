#pragma once
#include "ISystem.h"
#include "TransformComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "DirectionalLight.h"

namespace renderer
{
	struct LightingData;
}

namespace systems
{
	using PointLightArchetypeType = systems::ArchetypeDataTypes< forge::TransformComponentData, forge::PointLightComponentData >;
	using SpotLightArchetypeType = systems::ArchetypeDataTypes< forge::TransformComponentData, forge::SpotLightComponentData >;
	using DirectionalLightArchetypeType = systems::ArchetypeDataTypes< forge::DirectionalLightComponentData >;

	class LightingSystem : public ECSSystem< PointLightArchetypeType, SpotLightArchetypeType, DirectionalLightArchetypeType >
	{
	public:
		using ECSSystem::ECSSystem;

		virtual void OnInitialize();

		FORGE_INLINE forge::ArraySpan< const renderer::PointLightData > GetPointLights() const
		{
			return m_pointsLightsData;
		}

		FORGE_INLINE forge::ArraySpan< const renderer::SpotLightData > GetSpotLights() const
		{
			return m_spotLightsData;
		}

		FORGE_INLINE forge::ArraySpan< const renderer::DirectionalLightData > GetDirectionalLights() const
		{
			return m_directionalLightsData;
		}
		
		renderer::LightingData GetLightingData() const;

		FORGE_INLINE void SetAmbientColor( Vector3 ambientColor )
		{
			m_ambientColor = ambientColor;
		}

		FORGE_INLINE const Vector3& GetAmbientColor() const
		{
			return m_ambientColor;
		}

	private:
		void Update();

		forge::CallbackToken m_updateToken;
		std::vector< renderer::PointLightData > m_pointsLightsData;
		std::vector< renderer::SpotLightData > m_spotLightsData;
		std::vector< renderer::DirectionalLightData > m_directionalLightsData;
		Vector3 m_ambientColor;

#ifdef FORGE_DEBUGGING
		virtual const std::string& GetDebugFriendlyName() const { static std::string name = "Lighting System"; return name; }
#endif

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif
	};

}
