#pragma once
#include "ISystem.h"

namespace systems
{
	class TimeSystem : public ISystem
	{
	public:
		using ISystem::ISystem;

		virtual void OnInitialize();
		
		FORGE_INLINE float GetCurrentTime() const
		{
			return m_currentTime;
		}

		FORGE_INLINE float GetDeltaTime() const
		{
			return m_lastDeltaTime;
		}

		FORGE_INLINE float GetTimeDilation() const
		{
			return m_timeDilation;
		}

	private:
		void Update();
		forge::CallbackToken m_updateToken;

		float m_timeDilation = 1.0f;
		float m_lastDeltaTime = 0.0f;
		float m_currentTime = 0.0f;

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_overlayHandle;
		float m_paused = false;
#endif
	};
}
