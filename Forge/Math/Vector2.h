#pragma once

#include "build.h"

class MATH_API Vector2
{
public:
	Vector2();
	Vector2( Float x, Float y );
	~Vector2();

	Float SqrMagnitude();
	Float Magnitude();

	Float X;
	Float Y;
};

