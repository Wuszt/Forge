#include "fpch.h"

#define SPDLOG_USE_STD_FORMAT
#include "spdlog/spdlog.h"

#include "Logger.h"
#include <cstdarg>

namespace forge
{
	void Logger::LogToConsole( LogType type, const std::string& str )
	{
		static Bool attached = false;
		if ( !attached )
		{
			AttachConsole( ATTACH_PARENT_PROCESS );
			attached = true;
		}

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

	void Logger::LogInfo( const char* str, ... )
	{
		va_list argsList;
		va_start( argsList, str );
		LogToConsole( LogType::Info, forge::String::Printf( str, argsList ) );
		va_end( argsList );
	}

	void Logger::LogWarning( const char* str, ... )
	{
		va_list argsList;
		va_start( argsList, str );
		LogToConsole( LogType::Warning, forge::String::Printf( str, argsList ) );
		va_end( argsList );
	}

	void Logger::LogError( const char* str, ... )
	{
		va_list argsList;
		va_start( argsList, str );
		LogToConsole( LogType::Error, forge::String::Printf( str, argsList ) );
		va_end( argsList );
	}

}
