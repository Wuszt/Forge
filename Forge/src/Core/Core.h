#pragma once

#define FORGE_ASSERTIONS_ENABLED
#define FORGE_FATALS_ENABLED

#ifdef FORGE_ASSERTIONS_ENABLED
	#include <assert.h>
	#define FORGE_ASSERT( ... ) assert( __VA_ARGS__ )
#else
	#define FORGE_ASSERT( ... )
#endif

#ifdef FORGE_FATALS_ENABLED
	#include <assert.h>
	#define FORGE_FATAL( msg ) assert(((void)msg, false ))
#else
	#define FORGE_FATAL( msg )
#endif

#if _DEBUG
#define FORGE_DEBUG
#endif

#define FORGE_INLINE inline

#if _WIN32 || _WIN64
#define FORGE_PLATFORM_WINDOWS 
#endif

#ifdef FORGE_PLATFORM_WINDOWS
#define FORGE_IMGUI_ENABLED
#endif