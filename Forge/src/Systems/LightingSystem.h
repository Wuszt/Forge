#pragma once
#include "TransformComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "DirectionalLightComponent.h"
#include "../Renderer/LightData.h"
#include "IMGUISystem.h"
#include "IDebuggable.h"

namespace renderer
{
	struct LightingData;

#ifdef FORGE_IMGUI_ENABLED
	class ITexture;
#endif
}

namespace systems
{
	class LightingSystem : public ISystem, public forge::IDebuggable
	{
		DECLARE_POLYMORPHIC_CLASS( LightingSystem, systems::ISystem );

	public:
		using Super::Super;

		virtual void OnInitialize();

		forge::ArraySpan< const renderer::LightData< renderer::PointLightData > > GetPointLights() const
		{
			return m_pointsLightsData;
		}	
		
		forge::ArraySpan< renderer::LightData< renderer::PointLightData > > GetPointLights()
		{
			return m_pointsLightsData;
		}

		forge::ArraySpan< const renderer::LightData< renderer::SpotLightData > > GetSpotLights() const
		{
			return m_spotLightsData;
		}

		forge::ArraySpan< renderer::LightData< renderer::SpotLightData > > GetSpotLights()
		{
			return m_spotLightsData;
		}

		forge::ArraySpan< const renderer::LightData< renderer::DirectionalLightData > > GetDirectionalLights() const
		{
			return m_directionalLightsData;
		}

		forge::ArraySpan< renderer::LightData< renderer::DirectionalLightData > > GetDirectionalLights()
		{
			return m_directionalLightsData;
		}
		
		renderer::LightingData GetLightingData();

		void SetAmbientColor( Vector3 ambientColor )
		{
			m_ambientColor = ambientColor;
		}

		const Vector3& GetAmbientColor() const
		{
			return m_ambientColor;
		}

		void ForceShadowMapsRecreation()
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

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;

		Float m_depthBufferDenominator = std::numeric_limits< Float >::infinity();
		std::vector< std::unique_ptr< renderer::ITexture > > m_temporaryTextures;
#endif
	};

}
