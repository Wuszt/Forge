#include "fpch.h"
#include "Time.h"
#include "WindowsTime.h"

namespace forge
{
	TimeImpl& TimeImpl::GetTimeInstance()
	{
		static windows::WindowsTime instance;
		return instance;
	}
}