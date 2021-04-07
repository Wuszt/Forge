#include "Vector2.h"
#include <cmath>
#include <limits>


Vector2::Vector2()
{}


Vector2::Vector2( Float x, Float y )
	: X( x )
	, Y( y )
{}

Vector2::Vector2( Float xy )
	: X( xy )
	, Y( xy )
{}

Vector2::~Vector2()
{
}

Vector2 Vector2::ZEROS()
{
	return 0.0f;
}

Vector2 Vector2::ONES()
{
	return 1.0f;
}

Vector2 Vector2::PLUS_MAX()
{
	return std::numeric_limits< Float >::max();
}

Vector2 Vector2::PLUS_INF()
{
	return std::numeric_limits< Float >::infinity();
}

Vector2 Vector2::MINUS_MAX()
{
	return -std::numeric_limits< Float >::max();
}

Vector2 Vector2::MINUS_INF()
{
	return -std::numeric_limits< Float >::infinity();
}

Vector2 Vector2::EX()
{
	return Vector2( 1.0f, 0.0f );
}

Vector2 Vector2::EY()
{
	return Vector2( 0.0f, 1.0f );
}

Float Vector2::SquareMag() const
{
	return X * X + Y * Y;
}

Float Vector2::Mag() const
{
	return std::sqrt( X * X + Y * Y );
}

Float Vector2::Normalize()
{
	Float mag = Mag();
	X /= mag;
	Y /= mag;
	return mag;
}

Vector2 Vector2::Normalized() const
{
	Vector2 result = *this;
	result.Normalize();
	return result;
}

Float Vector2::Dot( const Vector2& vec ) const
{
	return X * vec.X + Y * vec.Y;
}

Vector2 Vector2::operator-() const
{
	return Vector2( -X, -Y );
}

Vector2 Vector2::operator+( const Vector2& vec ) const
{
	return Vector2( X + vec.X, Y + vec.Y );
}

void Vector2::operator+=( const Vector2& vec )
{
	X += vec.X;
	Y += vec.Y;
}

Bool Vector2::IsZero() const
{
	return X == 0.0f && Y == 0.0f;
}

Bool Vector2::IsAlmostZero() const
{
	return abs( X ) <= std::numeric_limits< decltype( X ) >::epsilon() && abs( Y ) <= std::numeric_limits< decltype( Y ) >::epsilon();
}

void Vector2::operator/=( Float val )
{
	X /= val;
	Y /= val;
}

void Vector2::operator*=( Float val )
{
	X *= val;
	Y *= val;
}

Bool Vector2::operator!=( const Vector2& vec ) const
{
	return X != vec.X || Y != vec.Y;
}

Bool Vector2::operator==( const Vector2& vec ) const
{
	return X == vec.X && Y == vec.Y;
}

void Vector2::operator-=( const Vector2& vec )
{
	X -= vec.X;
	Y -= vec.Y;
}
