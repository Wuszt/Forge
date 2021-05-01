#pragma once

#define FORGE_ASSERTIONS_ENABLED

#ifdef FORGE_ASSERTIONS_ENABLED
	#include <assert.h>
	#define FORGE_ASSERT( condition ) assert( condition )
#endif

#if _DEBUG
#define FORGE_DEBUG
#endif