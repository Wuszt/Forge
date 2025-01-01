#pragma once

struct Vector4;

struct Vector3
{
	RTTI_DECLARE_STRUCT( Vector3 );

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

	static Vector3 EX() { return Vector3( 1.0f, 0.0f, 0.0f ); }
	static Vector3 EY() { return Vector3( 0.0f, 1.0f, 0.0f ); }
	static Vector3 EZ() { return Vector3( 0.0f, 0.0f, 1.0f ); }
	static Vector3 ZEROS() { return Vector3(); }
	static Vector3 ONES() { return Vector3( 1.0f, 1.0f, 1.0f ); }
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

	Vector3 operator-() const
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

	Vector3 operator*( const Vector3& vec ) const
	{
		return Vector3( X * vec.X, Y * vec.Y, Z * vec.Z );
	}

	void operator*=( Float val )
	{
		X *= val;
		Y *= val;
		Z *= val;
	}

    Vector3 operator*( Float val ) const
    {
        return Vector3( X * val, Y * val, Z * val );
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
	Bool IsAlmostZero( Float eps = std::numeric_limits< Float >::epsilon() ) const;

	Float Mag() const
	{
		return std::sqrt( X * X + Y * Y + Z * Z );
	}
	Float SquareMag() const
	{ 
		return X * X + Y * Y + Z * Z; 
	}

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

	Bool IsOk() const
	{
		return abs( X ) != std::numeric_limits< Float >::infinity()
			&& abs( Y ) != std::numeric_limits< Float >::infinity()
			&& abs( Z ) != std::numeric_limits< Float >::infinity();
	}

   Bool IsAlmostEqual( const Vector3& vec, Float eps = std::numeric_limits< Float >::epsilon() * 10.0f ) const
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

   Float DistTo( const Vector3& vec ) const
   {
	   return ( *this - vec ).Mag();
   }

   Float SquareDistTo( const Vector3& vec ) const
   {
	   return ( *this - vec ).SquareMag();
   }

   const Vector2& AsVector2() const
   {
	   return vec2;
   }

   Vector2& AsVector2()
   {
	   return vec2;
   }

   const Float* AsArray() const
   {
	   return &X;
   }

   Float* AsArray()
   {
	   return &X;
   }

#ifndef FORGE_FINAL
   std::string ToDebugString( Uint32 precision = 2u ) const;
#endif
};

template<>
struct std::hash< Vector3 >
{
	std::size_t operator()( const Vector3& vec ) const noexcept;
};