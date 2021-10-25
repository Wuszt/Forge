#pragma once

#if _WIN32 || _WIN64
#define FORGE_PLATFORM_WINDOWS 
#endif

#if defined FORGE_DEBUG_VERSION

#define FORGE_DEBUGGING
#define FORGE_PROFILING_ENABLED
#define FORGE_GPU_DEBUGGING

#elif defined FORGE_RELEASE_VERSION

#define FORGE_DEBUGGING
#define FORGE_PROFILING_ENABLED

#elif defined FORGE_PROFILING_VERSION

#define FORGE_PROFILING_ENABLED

#elif defined FORGE_FINAL_VERSION

#endif

#ifdef FORGE_DEBUGGING

#define FORGE_ASSERTIONS_ENABLED
#define FORGE_FATALS_ENABLED
#define FORGE_ASSURES_ENABLED

#ifdef FORGE_PLATFORM_WINDOWS
#define FORGE_IMGUI_ENABLED
#endif

#endif

#ifdef FORGE_ASSERTIONS_ENABLED
#include <assert.h>
#define FORGE_ASSERT( ... ) assert( __VA_ARGS__ )
#else
#define NDEBUG
#define FORGE_ASSERT( ... )
#endif

#ifdef FORGE_FATALS_ENABLED
#include <assert.h>
#define FORGE_FATAL( msg ) assert(((void)msg, false ))
#else
#define NDEBUG
#define FORGE_FATAL( msg )
#endif

#define FORGE_INLINE inline

#ifdef FORGE_ASSURES_ENABLED
#include <assert.h>
#define FORGE_ASSURE( ... ) assert( __VA_ARGS__ )
#else
#define FORGE_ASSURE( ... ) __VA_ARGS__
#endif

#define FORGE_ARRAY_COUNT( arr ) sizeof( arr ) / sizeof( arr[ 0 ] )

namespace
{
	int testArr[ 5 ];
	static_assert( FORGE_ARRAY_COUNT( testArr ) == 5, "Array count is incorrect" );
}