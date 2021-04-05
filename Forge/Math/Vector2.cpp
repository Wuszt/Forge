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

Float Vector2::SqrMagnitude()
{
	return X * X + Y * Y;
}

Float Vector2::Magnitude()
{
	return std::sqrt( X * X + Y * Y );
}
