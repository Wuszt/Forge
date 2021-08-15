#include "fpch.h"
#include "WindowsTime.h"

namespace windows
{
	void WindowsTime::InitializeInternal()
	{
		QueryPerformanceFrequency( &m_frequency );
		QueryPerformanceCounter( &m_lastCounts );
	}

	float WindowsTime::GetTimeInternal()
	{
		return m_currentTime;
	}

	void WindowsTime::UpdateInternal()
	{
		LARGE_INTEGER currentCounts;

		QueryPerformanceCounter( &currentCounts );

		m_currentDeltaTime = (Float)( currentCounts.QuadPart - (Double)WindowsTime::m_lastCounts.QuadPart ) / WindowsTime::m_frequency.QuadPart;
		m_currentTime += m_currentDeltaTime;

		m_lastCounts = currentCounts;
	}

	float WindowsTime::GetDeltaTimeInternal()
	{
		return m_currentDeltaTime;
	}

	Float WindowsTime::GetRealTimeInternal()
	{
		LARGE_INTEGER currentCounts;

		QueryPerformanceCounter( &currentCounts );

		Float dt = (Float)( currentCounts.QuadPart - (Double)WindowsTime::m_lastCounts.QuadPart ) / WindowsTime::m_frequency.QuadPart;

		return m_currentTime + dt;
	}
}