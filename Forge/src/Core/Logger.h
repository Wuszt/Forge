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

		template< class... Args >
		static std::string CombineToString( const char* str, Args... args )
		{
			char buff[ 200 ];
			snprintf( buff, sizeof( buff ), str, args... );
			std::string buffAsStdStr = buff;

			return buffAsStdStr;
		}

		static std::string CombineToString( const char* str )
		{
			return str;
		}

	public:
		template< class... Args >
		static void LogInfo( const char* str, Args... args )
		{
			LogToConsole( LogType::Info, CombineToString( str, args... ) );
		}

		template< class... Args >
		static void LogWarning( const char* str, Args... args )
		{
			LogToConsole( LogType::Warning, CombineToString( str, std::forward< Args >( args )... ) );
		}

		template< class... Args >
		static void LogError( const char* str, Args... args )
		{
			LogToConsole( LogType::Error, CombineToString( str, std::forward< Args >( args )... ) );
		}
	};
}

