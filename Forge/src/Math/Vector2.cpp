#include "Fpch.h"
#include "Vector2.h"

RTTI_IMPLEMENT_TYPE( Vector2, 
	RTTI_REGISTER_PROPERTY( X, RTTI_ADD_METADATA( "Editable" ) );
	RTTI_REGISTER_PROPERTY( Y, RTTI_ADD_METADATA( "Editable" ) );
	);

Vector2 Vector2::PLUS_MAX()
{
	return Vector2( std::numeric_limits< Float >::max() );
}

Vector2 Vector2::PLUS_INF()
{
	return Vector2( std::numeric_limits< Float >::infinity() );
}

Vector2 Vector2::MINUS_MAX()
{
	return Vector2( -std::numeric_limits< Float >::max() );
}

Vector2 Vector2::MINUS_INF()
{
	return Vector2( -std::numeric_limits< Float >::infinity() );
}

#ifndef FORGE_FINAL
std::string Vector2::ToDebugString( Uint32 precision ) const
{
	std::string format = forge::String::Printf( "[%s%u%s, %s%u%s]", "%.", precision, "f", "%.", precision, "f" );
	return forge::String::Printf( format.c_str(), X, Y );
}
#endif
