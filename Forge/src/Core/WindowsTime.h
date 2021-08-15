#pragma once
#include <Windows.h>
#include "Time.h"

namespace windows
{
	class WindowsTime : public forge::TimeImpl
	{
	public:
		virtual void InitializeInternal() override;
		virtual Float GetTimeInternal() override;
		virtual void UpdateInternal() override;
		virtual Float GetDeltaTimeInternal() override;
		virtual Float GetRealTimeInternal() override;

	private:
		LARGE_INTEGER m_lastCounts;
		LARGE_INTEGER m_frequency;
		Float m_currentTime;
		Float m_currentDeltaTime;
	};
}