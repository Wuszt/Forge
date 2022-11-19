#pragma once


typedef float Float;
typedef __int32 Int32;
typedef __int64 Int64;
typedef bool Bool;
typedef unsigned short Uint16;
typedef unsigned Uint32;
typedef unsigned long long Uint64;
typedef double Double;
typedef char Char;
typedef __int8 Byte;

static_assert( sizeof( Float ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Double ) == 8, "Double should be the size of 4 bytes" );
static_assert( sizeof( Int32 ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Int64 ) == 8, "Int64 should be the size of 8 bytes" );
static_assert( sizeof( Bool ) == 1, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Uint16 ) == 2, "Uint16 should be the size of 2 bytes" );
static_assert( sizeof( Uint32 ) == 4, "Uint32 should be the size of 4 bytes" );
static_assert( sizeof( Uint64 ) == 8, "Uint64 should be the size of 8 bytes" );
static_assert( sizeof( Char ) == 1, "Char should be the size of 1 byte" );
static_assert( sizeof( Byte ) == 1, "Byte should be the size of 1 byte" );