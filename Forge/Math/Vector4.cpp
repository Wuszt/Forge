#include "Fpch.h"

#include "Vector4.h"

Vector4 Vector4::PLUS_MAX()
{
	return Vector4( std::numeric_limits< Float >::max() );
}

Vector4 Vector4::MINUS_MAX()
{
	return -Vector4( std::numeric_limits< Float >::max() );
}

Vector4 Vector4::PLUS_INF()
{
	return Vector4( std::numeric_limits< Float >::infinity() );
}

Vector4 Vector4::MINUS_INF()
{
	return -Vector4( std::numeric_limits< Float >::infinity() );
}

Bool Vector4::IsAlmostZero() const
{
	constexpr Float epsilon = std::numeric_limits< Float >::epsilon();

	return X <= epsilon && Y <= epsilon && Z <= epsilon && W <= W;
}

Float Vector4::Mag3() const
{
	return std::sqrt( X * X + Y * Y + Z * Z );
}

Float Vector4::Mag4() const
{
	return std::sqrt( X * X + Y * Y + Z * Z + W * W );
}

Bool Vector4::IsOk() const
{
	return abs( X ) != std::numeric_limits< Float >::infinity()
		&& abs( Y ) != std::numeric_limits< Float >::infinity()
		&& abs( Z ) != std::numeric_limits< Float >::infinity()
		&& abs( W ) != std::numeric_limits< Float >::infinity();
}
