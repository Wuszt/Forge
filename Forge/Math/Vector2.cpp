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

Float Vector2::Mag() const
{
	return std::sqrt( X * X + Y * Y );
}

Bool Vector2::IsAlmostZero() const
{
	return abs( X ) <= std::numeric_limits< decltype( X ) >::epsilon() && abs( Y ) <= std::numeric_limits< decltype( Y ) >::epsilon();
}
