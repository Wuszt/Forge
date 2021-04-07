#pragma once

namespace Forge
{
	typedef float Float;
	typedef int Int32;
	typedef bool Bool;
}

typedef Forge::Float Float;
typedef Forge::Int32 Int32;
typedef Forge::Bool Bool;


















static_assert( sizeof( Forge::Int32 ) == 4, "Int32 should be the size of 4 bytes" );