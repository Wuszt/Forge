#pragma once

struct Vector3;

struct Vector4
{
	union
	{
		struct
		{
			Float X;
			Float Y;
			Float Z;
		};

		Vector3 vec3;
	};

	Float W = 1.0f;

	FORGE_INLINE static Vector4 EX() { return Vector4( 1.0f, 0.0f, 0.0f, 0.0f ); }
	FORGE_INLINE static Vector4 EY() { return Vector4( 0.0f, 1.0f, 0.0f, 0.0f ); }
	FORGE_INLINE static Vector4 EZ() { return Vector4( 0.0f, 0.0f, 1.0f, 0.0f ); }
	FORGE_INLINE static Vector4 EW() { return Vector4( 0.0f, 0.0f, 0.0f, 1.0f ); }
	FORGE_INLINE static Vector4 ZEROS() { return Vector4( 0.0f, 0.0f, 0.0f, 0.0f ); }
	FORGE_INLINE static Vector4 ONES() { return Vector4( 1.0f, 1.0f, 1.0f, 1.0f ); }
	FORGE_INLINE static Vector4 PLUS_MAX() { return Vector4( std::numeric_limits< Float >::max() ); }
	FORGE_INLINE static Vector4 MINUS_MAX() { return -Vector4( std::numeric_limits< Float >::max() ); }
	FORGE_INLINE static Vector4 PLUS_INF() { return Vector4( std::numeric_limits< Float >::infinity() ); }
	FORGE_INLINE static Vector4 MINUS_INF() { return -Vector4( std::numeric_limits< Float >::infinity() ); }

	Vector4()
		: X( 0.0f )
		, Y( 0.0f )
		, Z( 0.0f )
	{}

	Vector4( Float x, Float y, Float z, Float w )
		: X( x )
		, Y( y )
		, Z( z )
		, W ( w )
	{}

	Vector4( Float x, Float y, Float z)
		: X( x )
		, Y( y )
		, Z( z )
	{}

	explicit Vector4( Float xyz )
		: X( xyz )
		, Y( xyz )
		, Z( xyz )
		, W( xyz )
	{}

	Vector4( const Vector3& vec );

	Vector4( const Vector3& vec, Float w );

	Vector4( const Float (&arr)[4] )
		: X( arr[ 0 ] )
		, Y( arr[ 1 ] )
		, Z( arr[ 2 ] )
		, W( arr[ 3 ] )
	{}

	~Vector4() {}

	FORGE_INLINE Vector4 operator-()
	{
		return Vector4( -X, -Y, -Z, -W );
	}

	FORGE_INLINE Vector4 operator+( const Vector4& vec ) const
	{
		return Vector4( X + vec.X, Y + vec.Y, Z + vec.Z, W + vec.W );
	}

	FORGE_INLINE Vector4 operator-( const Vector4& vec ) const
	{
		return Vector4( X - vec.X, Y - vec.Y, Z - vec.Z, W - vec.W );
	}

	FORGE_INLINE Vector4 operator*( Float val ) const
	{
		return Vector4( X * val, Y * val, Z * val, W * val );
	}

	FORGE_INLINE Vector4 operator/( Float val ) const
	{
		Float inv = 1.0f / val;

		return Vector4( X * inv, Y * inv, Z * inv, W * inv );
	}

	FORGE_INLINE Float& operator[]( Uint32 index ) const
	{
		FORGE_ASSERT( index < 4 );

		Float* ptr = const_cast< Float* >( &X );
		return *( ptr + index );
	}

	FORGE_INLINE void operator*=( Float val )
	{
		X *= val;
		Y *= val;
		Z *= val;
		W *= val;
	}

	FORGE_INLINE void operator/=( Float val )
	{
		X /= val;
		Y /= val;
		Z /= val;
		W /= val;
	}

	FORGE_INLINE void operator-=( const Vector4& vec )
	{
		X -= vec.X;
		Y -= vec.Y;
		Z -= vec.Z;
		W -= vec.W;
	}

	FORGE_INLINE void operator+=( const Vector4& vec )
	{
		X += vec.X;
		Y += vec.Y;
		Z += vec.Z;
		W += vec.W;
	}

	FORGE_INLINE Bool operator==( const Vector4& vec ) const
	{
		return X == vec.X && Y == vec.Y && vec.Z == Z && vec.W == vec.W;
	}

	FORGE_INLINE Bool operator!=( const Vector4& vec ) const
	{
		return X != vec.X || Y != vec.Y || vec.Z != Z || vec.W != W;
	}

	FORGE_INLINE Bool IsZero() const { return X == 0.0f && Y == 0.0f && Z == 0.0f && W == 0.0f; }
	Bool IsAlmostZero( Float eps = std::numeric_limits< Float >::epsilon() ) const;

	FORGE_INLINE Float Mag3() const
	{
		return std::sqrt( X * X + Y * Y + Z * Z );
	}

	FORGE_INLINE Float SquareMag3() const
	{
		return X * X + Y * Y + Z * Z; 
	}

	FORGE_INLINE Vector4 Normalized3() const
	{
		Float mag = Mag3();
		return Vector4( X / mag, Y / mag, Z / mag, W / mag );
	}

	FORGE_INLINE Float Normalize3()
	{
		Float mag = Mag3();

		X /= mag;
		Y /= mag;
		Z /= mag;
		W /= mag;

		return mag;
	}

	FORGE_INLINE Float Dot3( const Vector4& vec ) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z;
	}

	FORGE_INLINE Float Mag4() const
	{
		return std::sqrt( X * X + Y * Y + Z * Z + W * W );
	}
	FORGE_INLINE Float SquareMag4() const
	{
		return X * X + Y * Y + Z * Z + W * W; 
	}

	FORGE_INLINE Vector4 Normalized4() const
	{
		Float mag = Mag4();
		return Vector4( X / mag, Y / mag, Z / mag, W / mag );
	}

	FORGE_INLINE Float Normalize4()
	{
		Float mag = Mag4();

		X /= mag;
		Y /= mag;
		Z /= mag;
		W /= mag;

		return mag;
	}

	FORGE_INLINE Float Dot4( const Vector4& vec ) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z + W * vec.W;
	}

	FORGE_INLINE Bool IsOk() const
	{
		return abs( X ) != std::numeric_limits< Float >::infinity()
			&& abs( Y ) != std::numeric_limits< Float >::infinity()
			&& abs( Z ) != std::numeric_limits< Float >::infinity()
			&& abs( W ) != std::numeric_limits< Float >::infinity();
	}

	FORGE_INLINE Bool IsAlmostEqual( const Vector4& vec, Float eps = std::numeric_limits< Float >::epsilon() ) const
	{
		return ( *this - vec ).IsAlmostZero( eps );
	}

	FORGE_INLINE const Vector3& AsVector3() const
	{
		return vec3;
	}

	FORGE_INLINE const Float* AsArray() const
	{
		return &X;
	}

#ifdef FORGE_DEBUG
	std::string ToDebugString() const;
#endif
};

