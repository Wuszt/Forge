#pragma once

namespace forge
{
	class TimeImpl
	{
	public:
		static TimeImpl& GetTimeInstance();

		virtual ~TimeImpl() {}

		virtual void InitializeInternal() = 0;
		virtual Float GetTimeInternal() = 0;
		virtual Float GetRealTimeInternal() = 0;
		virtual void UpdateInternal() = 0;
		virtual Float GetDeltaTimeInternal() = 0;
	};

	class Time
	{
	public:
		static FORGE_INLINE Float GetTime()
		{
			return TimeImpl::GetTimeInstance().GetTimeInternal();
		}

		static FORGE_INLINE void Initialize()
		{
			TimeImpl::GetTimeInstance().InitializeInternal();
		}

		static FORGE_INLINE void Update()
		{
			TimeImpl::GetTimeInstance().UpdateInternal();
		}

		static FORGE_INLINE Float GetDeltaTime()
		{
			return TimeImpl::GetTimeInstance().GetDeltaTimeInternal();
		}

		static FORGE_INLINE Float GetRealTime()
		{
			return TimeImpl::GetTimeInstance().GetRealTimeInternal();
		}
	};

	class StopWatch
	{
	public:
		FORGE_INLINE StopWatch()
		{
			Reset();
		}

		FORGE_INLINE void Reset()
		{
			m_start = Time::GetRealTime();
		}

		FORGE_INLINE Float GetDuration() const
		{
			return Time::GetRealTime() - m_start;
		}

	private:
		Float m_start = 0.0f;
	};
}

