#pragma once

struct Vector2
{
	Float X = 0.0f;
	Float Y = 0.0f;

	Vector2() {}

	Vector2( Float x, Float y )
		: X( x )
		, Y( y )
	{}

	explicit Vector2( Float xy )
		: X( xy )
		, Y( xy )
	{}

	~Vector2() {}

	FORGE_INLINE static Vector2 ZEROS() { return Vector2( 0.0f ); }
	FORGE_INLINE static Vector2 ONES() { return Vector2( 1.0f ); }
	static Vector2 PLUS_MAX();
	static Vector2 PLUS_INF();
	static Vector2 MINUS_MAX();
	static Vector2 MINUS_INF();
	FORGE_INLINE static Vector2 EX() { return Vector2( 1.0f, 0.0f ); }
	FORGE_INLINE static Vector2 EY() { return Vector2( 0.0f, 1.0f ); }

	FORGE_INLINE Float SquareMag() const { return X * X + Y * Y; }

	FORGE_INLINE Float Mag() const
	{
		return std::sqrt( X * X + Y * Y );
	}

	FORGE_INLINE Float Normalize()
	{
		Float mag = Mag();
		X /= mag;
		Y /= mag;
		return mag;
	}

	FORGE_INLINE Float DistanceTo( const Vector2& to ) const
	{
		return ( *this - to ).Mag();
	}

	FORGE_INLINE Float DistanceSquaredTo( const Vector2& to ) const
	{
		return ( *this - to ).SquareMag();
	}

	FORGE_INLINE Vector2 Normalized() const
	{
		Vector2 result = *this;
		result.Normalize();
		return result;
	}

	FORGE_INLINE Float Dot( const Vector2& vec ) const { return X * vec.X + Y * vec.Y; }

	FORGE_INLINE Vector2 operator-() const { return Vector2( -X, -Y ); }

	FORGE_INLINE Vector2 operator*( Float value ) const { return Vector2( value * X, value * Y ); }
	FORGE_INLINE Vector2 operator+( const Vector2& vec ) const { return Vector2( X + vec.X, Y + vec.Y ); }
	FORGE_INLINE Vector2 operator-( const Vector2& vec ) const { return Vector2( X - vec.X, Y - vec.Y ); }

	FORGE_INLINE void operator+=( const Vector2& vec )
	{
		X += vec.X;
		Y += vec.Y;
	}

	FORGE_INLINE void operator-=( const Vector2& vec )
	{
		X -= vec.X;
		Y -= vec.Y;
	}

	FORGE_INLINE void operator*=( Float val )
	{
		X *= val;
		Y *= val;
	}

	FORGE_INLINE void operator/=( Float val )
	{
		X /= val;
		Y /= val;
	}

	FORGE_INLINE Bool operator==( const Vector2& vec ) const { return X == vec.X && Y == vec.Y; }

	FORGE_INLINE Bool operator!=( const Vector2& vec ) const { return X != vec.X || Y != vec.Y; }

	FORGE_INLINE Bool IsZero() const { return X == 0.0f && Y == 0.0f; }
	FORGE_INLINE Bool IsAlmostZero() const
	{
		return abs( X ) <= std::numeric_limits< decltype( X ) >::epsilon() && abs( Y ) <= std::numeric_limits< decltype( Y ) >::epsilon();
	}

#ifndef FORGE_FINAL
	std::string ToDebugString( Uint32 precision = 2u ) const;
#endif
};

