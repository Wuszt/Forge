#pragma once

namespace systems
{
	class TimeSystem : public ISystem
	{
		DECLARE_POLYMORPHIC_CLASS( TimeSystem, systems::ISystem );
	public:
		using ISystem::ISystem;

		virtual void OnInitialize();
		
		Float GetCurrentTime() const
		{
			return m_currentTime;
		}

		Float GetDeltaTime() const
		{
			return m_lastDeltaTime;
		}

		Float GetTimeDilation() const
		{
			return m_timeDilation;
		}

	private:
		void Update();
		forge::CallbackToken m_updateToken;

		Float m_timeDilation = 1.0f;
		Float m_lastDeltaTime = 0.0f;
		Float m_currentTime = 0.0f;

#ifdef FORGE_IMGUI_ENABLED
		forge::CallbackToken m_overlayHandle;
		Float m_paused = false;
#endif
	};
}
