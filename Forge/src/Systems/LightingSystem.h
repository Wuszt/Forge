#pragma once
#include "ISystem.h"
#include "TransformComponent.h"
#include "LightComponent.h"

namespace systems
{
	class LightingSystem : public ECSSystem< forge::TransformComponentData, forge::LightComponentData >
	{
	public:
		using ECSSystem::ECSSystem;

		virtual void OnInitialize();

		forge::ArraySpan< const renderer::LightData > GetLights() const;
		
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
		std::vector< renderer::LightData > m_lightsData;
		Vector3 m_ambientColor;

	protected:
#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif
	};

}
