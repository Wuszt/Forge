#include "fpch.h"
#include "StringUtils.h"
#include <stdarg.h>
#include <stdio.h>

namespace forge
{
	std::string String::Printf( const char* txt, ... )
	{
		va_list argsList;
		va_start( argsList, txt );

		std::string result = Printf( txt, argsList );
		va_end( argsList );
		return result;
	}

	std::string String::Printf( const char* txt, const va_list& argsList )
	{
		char buff[ 4096 ];
		::vsnprintf_s( buff, FORGE_ARRAY_COUNT( buff ), txt, argsList );
		return std::string( buff );
	}

}
