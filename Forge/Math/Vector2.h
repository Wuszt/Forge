#pragma once

#include "build.h"

class MATH_API Vector2
{
public:
	Vector2();
	Vector2( Float x, Float y );
	Vector2( Float xy );
	~Vector2();

	static Vector2 ZEROS();
	static Vector2 ONES();
	static Vector2 PLUS_MAX();
	static Vector2 PLUS_INF();
	static Vector2 MINUS_MAX();
	static Vector2 MINUS_INF();
	static Vector2 EX();
	static Vector2 EY();

	Float SquareMag() const;
	Float Mag() const;
	Float Normalize();
	Vector2 Normalized() const;
	Float Dot( const Vector2& vec ) const;

	Vector2 operator-() const;

	Vector2 operator+( const Vector2& vec ) const;

	void operator+=( const Vector2& vec );
	void operator-=( const Vector2& vec );
	void operator*=( Float val );
	void operator/=( Float val );

	Bool operator==( const Vector2& vec ) const;
	Bool operator!=( const Vector2& vec ) const;

	Bool IsZero() const;
	Bool IsAlmostZero() const;

	Float X;
	Float Y;
};

