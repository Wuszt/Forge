#include "fpch.h"
#include "spdlog/spdlog.h"
#include "Logger.h"

void Logger::LogToConsole( LogType type, const std::string& str )
{
	if( type == LogType::Info )
	{
		spdlog::info( str );
	}
	else if( type == LogType::Warning )
	{
		spdlog::warn( str );
	}
	else if( type == LogType::Error )
	{
		spdlog::error( str );
	}
}
