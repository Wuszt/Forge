#pragma once


typedef float Float;
typedef int32_t Int32;
typedef int64_t Int64;
typedef bool Bool;
typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef double Double;
typedef char Char;
typedef uint8_t Byte;

static_assert( sizeof( Float ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Double ) == 8, "Double should be the size of 4 bytes" );
static_assert( sizeof( Int32 ) == 4, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Int64 ) == 8, "Int64 should be the size of 8 bytes" );
static_assert( sizeof( Bool ) == 1, "Int32 should be the size of 4 bytes" );
static_assert( sizeof( Uint8 ) == 1, "Uint16 should be the size of 2 bytes" );
static_assert( sizeof( Uint16 ) == 2, "Uint16 should be the size of 2 bytes" );
static_assert( sizeof( Uint32 ) == 4, "Uint32 should be the size of 4 bytes" );
static_assert( sizeof( Uint64 ) == 8, "Uint64 should be the size of 8 bytes" );
static_assert( sizeof( Char ) == 1, "Char should be the size of 1 byte" );
static_assert( sizeof( Byte ) == 1, "Byte should be the size of 1 byte" );