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
		static Float GetTime()
		{
			return TimeImpl::GetTimeInstance().GetTimeInternal();
		}

		static void Initialize()
		{
			TimeImpl::GetTimeInstance().InitializeInternal();
		}

		static void Update()
		{
			TimeImpl::GetTimeInstance().UpdateInternal();
		}

		static Float GetDeltaTime()
		{
			return TimeImpl::GetTimeInstance().GetDeltaTimeInternal();
		}

		static Float GetRealTime()
		{
			return TimeImpl::GetTimeInstance().GetRealTimeInternal();
		}
	};

	class StopWatch
	{
	public:
		StopWatch()
		{
			Reset();
		}

		void Reset()
		{
			m_start = Time::GetRealTime();
		}

		Float GetDuration() const
		{
			return Time::GetRealTime() - m_start;
		}

	private:
		Float m_start = 0.0f;
	};
}

