#pragma once
#include "ISystem.h"
#include "TransformComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "DirectionalLight.h"
#include "../Renderer/LightData.h"

namespace renderer
{
	struct LightingData;

#ifdef FORGE_IMGUI_ENABLED
	class ITexture;
#endif
}

namespace systems
{
	using PointLightArchetypeType = systems::ArchetypeDataTypes< forge::TransformComponentData, forge::PointLightComponentData >;
	using SpotLightArchetypeType = systems::ArchetypeDataTypes< forge::TransformComponentData, forge::SpotLightComponentData >;
	using DirectionalLightArchetypeType = systems::ArchetypeDataTypes< forge::DirectionalLightComponentData >;

	class LightingSystem : public ECSSystem< PointLightArchetypeType, SpotLightArchetypeType, DirectionalLightArchetypeType >
	{
		DECLARE_TYPE( LightingSystem, systems, IECSSystem );

	public:
		using ECSSystem::ECSSystem;

		virtual void OnInitialize();

		FORGE_INLINE forge::ArraySpan< const renderer::LightData< renderer::PointLightData > > GetPointLights() const
		{
			return m_pointsLightsData;
		}	
		
		FORGE_INLINE forge::ArraySpan< renderer::LightData< renderer::PointLightData > > GetPointLights()
		{
			return m_pointsLightsData;
		}

		FORGE_INLINE forge::ArraySpan< const renderer::LightData< renderer::SpotLightData > > GetSpotLights() const
		{
			return m_spotLightsData;
		}

		FORGE_INLINE forge::ArraySpan< renderer::LightData< renderer::SpotLightData > > GetSpotLights()
		{
			return m_spotLightsData;
		}

		FORGE_INLINE forge::ArraySpan< const renderer::LightData< renderer::DirectionalLightData > > GetDirectionalLights() const
		{
			return m_directionalLightsData;
		}

		FORGE_INLINE forge::ArraySpan< renderer::LightData< renderer::DirectionalLightData > > GetDirectionalLights()
		{
			return m_directionalLightsData;
		}
		
		renderer::LightingData GetLightingData();

		FORGE_INLINE void SetAmbientColor( Vector3 ambientColor )
		{
			m_ambientColor = ambientColor;
		}

		FORGE_INLINE const Vector3& GetAmbientColor() const
		{
			return m_ambientColor;
		}

		FORGE_INLINE void ForceShadowMapsRecreation()
		{
			m_shadowMapsRecreationForced = true;
		}

	private:
		void Update();

		forge::CallbackToken m_updateToken;
		std::vector< renderer::LightData< renderer::PointLightData > > m_pointsLightsData;
		std::vector< renderer::LightData< renderer::SpotLightData > > m_spotLightsData;
		std::vector< renderer::LightData< renderer::DirectionalLightData > > m_directionalLightsData;
		Vector3 m_ambientColor;
		Float m_shadowsResolutionScale = 1.0f;
		Bool m_shadowMapsRecreationForced = false;

#ifdef FORGE_DEBUGGING
		virtual const std::string& GetDebugFriendlyName() const { static std::string name = "Lighting System"; return name; }
#endif

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;

		Float m_depthBufferDenominator = std::numeric_limits< Float >::infinity();
		std::vector< std::unique_ptr< renderer::ITexture > > m_temporaryTextures;
#endif
	};

}
