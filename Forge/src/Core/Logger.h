#pragma once
#include <sstream>

#define FORGE_LOG( ... ) forge::Logger::LogInfo( ##__VA_ARGS__ )
#define FORGE_LOG_WARNING( ... ) forge::Logger::LogWarning( ##__VA_ARGS__ )
#define FORGE_LOG_ERROR( ... ) forge::Logger::LogError( ##__VA_ARGS__ )

namespace forge
{
	class Logger
	{
	private:

		enum class LogType
		{
			Info,
			Warning,
			Error
		};

		static void LogToConsole( LogType type, const std::string& str );

	public:
		static void LogInfo( const char* str, ... );

		static void LogWarning( const char* str, ... );

		static void LogError( const char* str, ... );
	};
}

