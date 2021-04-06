#include "Vector2.h"
#include <cmath>


Vector2::Vector2()
{}


Vector2::Vector2( Float x, Float y )
	: X( x )
	, Y( y )
{}

Vector2::~Vector2()
{
}

Float Vector2::SqrMagnitude() const
{
	return X * X + Y * Y;
}

Float Vector2::Magnitude() const
{
	return std::sqrt( X * X + Y * Y );
}
