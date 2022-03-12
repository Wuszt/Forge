#pragma once
#include "ISystem.h"
#include "TransformComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"

namespace renderer
{
	struct LightingData;
}

namespace systems
{
	using PointLightArchetypeType = systems::ArchetypeDataTypes< forge::TransformComponentData, forge::PointLightComponentData >;
	using SpotLightArchetypeType = systems::ArchetypeDataTypes< forge::TransformComponentData, forge::SpotLightComponentData >;

	class LightingSystem : public ECSSystem< PointLightArchetypeType, SpotLightArchetypeType >
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
		Vector3 m_ambientColor;

#ifdef FORGE_DEBUGGING
		virtual const std::string& GetDebugFriendlyName() const { static std::string name = "Lighting System"; return name; }
#endif

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif
	};

}
