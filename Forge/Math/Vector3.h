#pragma once
#include "Fpch.h"

class Vector4;

class Vector3
{
public:

	Float X = 0.0f;
	Float Y = 0.0f;
	Float Z = 0.0f;

	FORGE_INLINE static Vector3 EX() { return Vector3( 1.0f, 0.0f, 0.0f ); }
	FORGE_INLINE static Vector3 EY() { return Vector3( 0.0f, 1.0f, 0.0f ); }
	FORGE_INLINE static Vector3 EZ() { return Vector3( 0.0f, 0.0f, 1.0f ); }
	FORGE_INLINE static Vector3 ZEROS() { return Vector3(); }
	FORGE_INLINE static Vector3 ONES() { return Vector3( 1.0f, 1.0f, 1.0f ); }
	FORGE_INLINE static Vector3 PLUS_MAX() { return Vector3( std::numeric_limits< Float >::max() ); }
	FORGE_INLINE static Vector3 MINUS_MAX() { return -Vector3( std::numeric_limits< Float >::max() ); }
	FORGE_INLINE static Vector3 PLUS_INF() { return Vector3( std::numeric_limits< Float >::infinity() ); }
	FORGE_INLINE static Vector3 MINUS_INF() { return -Vector3( std::numeric_limits< Float >::infinity() ); }

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

	Vector3( const Vector4& vec );

	~Vector3(){}

	FORGE_INLINE Vector3 operator-()
	{
		return Vector3( -X, -Y, -Z );
	}

	FORGE_INLINE Vector3 operator+( const Vector3& vec ) const
	{
		return Vector3( X + vec.X, Y + vec.Y, Z + vec.Z );
	}

	FORGE_INLINE Vector3 operator-( const Vector3& vec ) const
	{
		return Vector3( X - vec.X, Y - vec.Y, Z - vec.Z );
	}

	FORGE_INLINE void operator*=( Float val )
	{
		X *= val;
		Y *= val;
		Z *= val;
	}

	FORGE_INLINE void operator/=( Float val )
	{
		X /= val;
		Y /= val;
		Z /= val;
	}

	FORGE_INLINE void operator-=( const Vector3& vec )
	{
		X -= vec.X;
		Y -= vec.Y;
		Z -= vec.Z;
	}

	FORGE_INLINE void operator+=( const Vector3& vec )
	{
		X += vec.X;
		Y += vec.Y;
		Z += vec.Z;
	}

	FORGE_INLINE Bool operator==( const Vector3& vec ) const
	{
		return X == vec.X && Y == vec.Y && vec.Z == Z;
	}

	FORGE_INLINE Bool operator!=( const Vector3& vec ) const
	{
		return X != vec.X || Y != vec.Y || vec.Z != Z;
	}

	FORGE_INLINE Bool IsZero() const { return X == 0.0f && Y == 0.0f && Z == 0.0f; }
	FORGE_INLINE Bool IsAlmostZero() const
	{
		constexpr Float epsilon = std::numeric_limits< Float >::epsilon();

		return X <= epsilon && Y <= epsilon && Z <= epsilon;
	}

	FORGE_INLINE Float Mag() const
	{
		return std::sqrt( X * X + Y * Y + Z * Z );
	}
	FORGE_INLINE Float SquareMag() const
	{ 
		return X * X + Y * Y + Z * Z; 
	}

	FORGE_INLINE Vector3 Normalized() const
	{
		Float mag = Mag();
		return Vector3( X / mag, Y / mag, Z / mag );
	}

	FORGE_INLINE Float Normalize()
	{
		Float mag = Mag();

		X /= mag;
		Y /= mag;
		Z /= mag;

		return mag;
	}

	FORGE_INLINE Float Dot( const Vector3& vec ) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z;
	}

	FORGE_INLINE Bool IsOk() const
	{
		return abs( X ) != std::numeric_limits< Float >::infinity()
			&& abs( Y ) != std::numeric_limits< Float >::infinity()
			&& abs( Z ) != std::numeric_limits< Float >::infinity();
	}
};

