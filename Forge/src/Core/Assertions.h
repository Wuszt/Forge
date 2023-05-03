#pragma once
#include <iostream>
#include <cstdlib>

static void Assert( bool expression, const char* expressionName, const char* fileName, unsigned lineNumber, const char* msg = nullptr )
{
	if ( !expression )
    {
            std::cerr << "Assertion failed: " << expressionName << " in " << fileName << " line " << lineNumber; \
            if( msg )
            {
                std::cerr << msg;
            }

            std::cerr << "\n";

			std::abort();
	}
}

#ifdef FORGE_ASSERTIONS_ENABLED
#define FORGE_ASSERT(expression, ...) \
        do { \
            Assert( !!(##expression##), #expression, __FILE__, __LINE__, ##__VA_ARGS__ ); \
        } while (false)
#else
#define FORGE_ASSERT(expression, ...) ((void)0)
#endif

#ifdef FORGE_FATALS_ENABLED
#define FORGE_FATAL( ... ) FORGE_ASSERT( false, __VA_ARGS__ )
#else
#define FORGE_FATAL( ... )
#endif

#ifdef FORGE_ASSURES_ENABLED
#define FORGE_ASSURE( expr, ... ) FORGE_ASSERT( expr, __VA_ARGS__ )
#else
#define FORGE_ASSURE( expr, ... ) expr
#endif