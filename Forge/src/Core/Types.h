#pragma once


typedef float Float;
typedef int Int32;
typedef bool Bool;
typedef unsigned Uint32;
typedef unsigned long long Uint64;
typedef double Double;
typedef char Char;

static_assert( sizeof( Float ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Double ) == 8, "Double should be the size of 4 bytes" );
static_assert( sizeof( Int32 ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Bool ) == 1, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Uint32 ) == 4, "Uint32 should be the size of 4 bytes" );
static_assert( sizeof( Uint64 ) == 8, "Uint64 should be the size of 8 bytes" );
static_assert( sizeof( Char ) == 1, "Char should be the size of 1 byte" );