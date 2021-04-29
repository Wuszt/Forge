#pragma once
#include "Fpch.h"

class Vector3
{
public:

	Float X = 0.0f;
	Float Y = 0.0f;
	Float Z = 0.0f;

	static Vector3 EX() { return Vector3( 1.0f, 0.0f, 0.0f ); }
	static Vector3 EY() { return Vector3( 0.0f, 1.0f, 0.0f ); }
	static Vector3 EZ() { return Vector3( 0.0f, 0.0f, 1.0f ); }
	static Vector3 ZEROS() { return Vector3(); }
	static Vector3 ONES() { return Vector3( 1.0f, 1.0f, 1.0f ); }
	static Vector3 PLUS_MAX();
	static Vector3 MINUS_MAX();
	static Vector3 PLUS_INF();
	static Vector3 MINUS_INF();

	Vector3() {}

	Vector3( Float x, Float y, Float z )
		: X( x )
		, Y( y )
		, Z( z )
	{}

	explicit Vector3( Float xyz )
		: X( xyz )
		, Y( xyz )
		, Z( xyz )
	{}

	~Vector3(){}

	Vector3 operator-()
	{
		return Vector3( -X, -Y, -Z );
	}

	Vector3 operator+( const Vector3& vec ) const
	{
		return Vector3( X + vec.X, Y + vec.Y, Z + vec.Z );
	}

	Vector3 operator-( const Vector3& vec ) const
	{
		return Vector3( X - vec.X, Y - vec.Y, Z - vec.Z );
	}

	void operator*=( Float val )
	{
		X *= val;
		Y *= val;
		Z *= val;
	}

	void operator/=( Float val )
	{
		X /= val;
		Y /= val;
		Z /= val;
	}

	void operator-=( const Vector3& vec )
	{
		X -= vec.X;
		Y -= vec.Y;
		Z -= vec.Z;
	}

	void operator+=( const Vector3& vec )
	{
		X += vec.X;
		Y += vec.Y;
		Z += vec.Z;
	}

	Bool operator==( const Vector3& vec ) const
	{
		return X == vec.X && Y == vec.Y && vec.Z == Z;
	}

	Bool operator!=( const Vector3& vec ) const
	{
		return X != vec.X || Y != vec.Y || vec.Z != Z;
	}

	Bool IsZero() const { return X == 0.0f && Y == 0.0f && Z == 0.0f; }
	Bool IsAlmostZero() const;

	Float Mag() const;
	Float SquareMag() const { return X * X + Y * Y + Z * Z; }

	Vector3 Normalized() const
	{
		Float mag = Mag();
		return Vector3( X / mag, Y / mag, Z / mag );
	}

	Float Normalize()
	{
		Float mag = Mag();

		X /= mag;
		Y /= mag;
		Z /= mag;

		return mag;
	}

	Float Dot( const Vector3& vec ) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z;
	}

	Bool IsOk() const;
};

