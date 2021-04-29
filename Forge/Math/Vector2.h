#pragma once
#include "Fpch.h"

class Vector2
{
public:

	Float X;
	Float Y;

	Vector2() {}

	Vector2( Float x, Float y )
		: X( x )
		, Y( y )
	{}

	Vector2( Float xy )
		: X( xy )
		, Y( xy )
	{}

	~Vector2() {}

	static Vector2 ZEROS() { return 0.0f; }
	static Vector2 ONES() { return 1.0f; }
	static Vector2 PLUS_MAX();
	static Vector2 PLUS_INF();
	static Vector2 MINUS_MAX();
	static Vector2 MINUS_INF();
	static Vector2 EX() { return Vector2( 1.0f, 0.0f ); }
	static Vector2 EY() { return Vector2( 0.0f, 1.0f ); }

	Float SquareMag() const { return X * X + Y * Y; }

	Float Mag() const;

	Float Normalize()
	{
		Float mag = Mag();
		X /= mag;
		Y /= mag;
		return mag;
	}

	Vector2 Normalized() const
	{
		Vector2 result = *this;
		result.Normalize();
		return result;
	}

	Float Dot( const Vector2& vec ) const { return X * vec.X + Y * vec.Y; }

	Vector2 operator-() const { return Vector2( -X, -Y ); }

	Vector2 operator+( const Vector2& vec ) const { return Vector2( X + vec.X, Y + vec.Y ); }

	void operator+=( const Vector2& vec )
	{
		X += vec.X;
		Y += vec.Y;
	}

	void operator-=( const Vector2& vec )
	{
		X -= vec.X;
		Y -= vec.Y;
	}

	void operator*=( Float val )
	{
		X *= val;
		Y *= val;
	}

	void operator/=( Float val )
	{
		X /= val;
		Y /= val;
	}

	Bool operator==( const Vector2& vec ) const { return X == vec.X && Y == vec.Y; }

	Bool operator!=( const Vector2& vec ) const { return X != vec.X || Y != vec.Y; }

	Bool IsZero() const { return X == 0.0f && Y == 0.0f; }
	Bool IsAlmostZero() const;
};

