#pragma once
#pragma comment(linker, "/WHOLEARCHIVE:Profiling.lib")

#ifdef FORGE_PROFILING_ENABLED

#define TRACY_ENABLE
#include "../../External/Tracy/public/tracy/Tracy.hpp"

#define PC_SCOPE( name ) ZoneScopedN( name )
#define PC_SCOPE_FUNC() ZoneScoped
#define PC_FRAME_END() FrameMark

#else

#define PC_SCOPE( name )
#define PC_SCOPE_FUNC()
#define PC_FRAME_END()

#endif