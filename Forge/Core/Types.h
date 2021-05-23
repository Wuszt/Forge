#pragma once

namespace Forge
{
	typedef float Float;
	typedef int Int32;
	typedef bool Bool;
	typedef unsigned Uint32;
	typedef unsigned long long Uint64;
	typedef double Double;
}

typedef Forge::Bool Bool;
typedef Forge::Float Float;
typedef Forge::Double Double;
typedef Forge::Int32 Int32;
typedef Forge::Uint32 Uint32;
typedef Forge::Uint64 Uint64;
















static_assert( sizeof( Forge::Float ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Forge::Double ) == 8, "Double should be the size of 4 bytes" );
static_assert( sizeof( Forge::Int32 ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Forge::Bool ) == 1, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Forge::Uint32 ) == 4, "Uint32 should be the size of 4 bytes" );
static_assert( sizeof( Forge::Uint64 ) == 8, "Uint64 should be the size of 8 bytes" );