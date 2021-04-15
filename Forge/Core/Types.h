#pragma once

namespace Forge
{
	typedef float Float;
	typedef int Int32;
	typedef bool Bool;
	typedef unsigned Uint32;
}

typedef Forge::Float Float;
typedef Forge::Int32 Int32;
typedef Forge::Bool Bool;
typedef Forge::Uint32 Uint32;

















static_assert( sizeof( Forge::Float ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Forge::Int32 ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Forge::Bool ) == 1, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Forge::Uint32 ) == 4, "Int32 should be the size of 4 bytes" );