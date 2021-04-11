#include "Vector3.h"
#include <limits>

Vector3 Vector3::PLUS_MAX()
{
	return Vector3( std::numeric_limits< Float >::max() );
}

Vector3 Vector3::MINUS_MAX()
{
	return -Vector3( std::numeric_limits< Float >::max() );
}

Vector3 Vector3::PLUS_INF()
{
	return Vector3( std::numeric_limits< Float >::infinity() );
}

Vector3 Vector3::MINUS_INF()
{
	return -Vector3( std::numeric_limits< Float >::infinity() );
}

Bool Vector3::IsAlmostZero() const
{
	constexpr Float epsilon = std::numeric_limits< Float >::epsilon();

	return X <= epsilon && Y <= epsilon && Z <= epsilon;
}

Float Vector3::Mag() const
{
	return std::sqrt( X * X + Y * Y + Z * Z );
}

Bool Vector3::IsOk() const
{
	return abs( X ) != std::numeric_limits< Float >::infinity()
		&& abs( Y ) != std::numeric_limits< Float >::infinity()
		&& abs( Z ) != std::numeric_limits< Float >::infinity();
}
