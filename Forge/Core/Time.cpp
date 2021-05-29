#include "fpch.h"
#include "Time.h"
#include "WindowsTime.h"

TimeImpl& TimeImpl::GetTimeInstance()
{
	static WindowsTime instance;
	return instance;
}