#include "Fpch.h"
#include "Vector2.h"

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

#ifdef FORGE_DEBUG
std::string Vector2::ToDebugString() const
{
	std::ostringstream stringStream;
	stringStream << "[" << X << ", " << Y << "]";
	return stringStream.str();
}
#endif
