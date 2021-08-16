#pragma once

struct Vector4;

struct Vector3
{
	union
	{
		struct
		{
			Float X;
			Float Y;
		};
		Vector2 vec2;
	};

	Float Z;

	FORGE_INLINE static Vector3 EX() { return Vector3( 1.0f, 0.0f, 0.0f ); }
	FORGE_INLINE static Vector3 EY() { return Vector3( 0.0f, 1.0f, 0.0f ); }
	FORGE_INLINE static Vector3 EZ() { return Vector3( 0.0f, 0.0f, 1.0f ); }
	FORGE_INLINE static Vector3 ZEROS() { return Vector3(); }
	FORGE_INLINE static Vector3 ONES() { return Vector3( 1.0f, 1.0f, 1.0f ); }
	static Vector3 PLUS_MAX();
	static Vector3 MINUS_MAX();
	static Vector3 PLUS_INF();
	static Vector3 MINUS_INF();

	Vector3()
	: X( 0.0f )
	, Y( 0.0f )
	, Z( 0.0f )
	{}

	Vector3( Vector2 vec, Float z )
		: X( vec.X )
		, Y( vec.Y )
		, Z( z )
	{}

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

	FORGE_INLINE Vector3 operator-() const
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

    FORGE_INLINE Vector3 operator*( Float val ) const
    {
        return Vector3( X * val, Y * val, Z * val );
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
	Bool IsAlmostZero( Float eps = std::numeric_limits< Float >::epsilon() ) const;

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

   FORGE_INLINE Bool IsAlmostEqual( const Vector3& vec, Float eps = std::numeric_limits< Float >::epsilon() * 10.0f ) const
   {
	   return ( *this - vec ).IsAlmostZero( eps );
   }

   Vector3 Cross( const Vector3& vec ) const
   {
	   return Vector3(
		   Y * vec.Z - Z * vec.Y,
		   Z * vec.X - X * vec.Z,
		   X * vec.Y - Y * vec.X );
   }

   FORGE_INLINE const Vector2& AsVector2() const
   {
	   return vec2;
   }

#ifdef FORGE_DEBUG
   std::string ToDebugString() const;
#endif
};

