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

	static Vector4 EX() { return Vector4( 1.0f, 0.0f, 0.0f, 0.0f ); }
	static Vector4 EY() { return Vector4( 0.0f, 1.0f, 0.0f, 0.0f ); }
	static Vector4 EZ() { return Vector4( 0.0f, 0.0f, 1.0f, 0.0f ); }
	static Vector4 EW() { return Vector4( 0.0f, 0.0f, 0.0f, 1.0f ); }
	static Vector4 ZEROS() { return Vector4( 0.0f, 0.0f, 0.0f, 0.0f ); }
	static Vector4 ONES() { return Vector4( 1.0f, 1.0f, 1.0f, 1.0f ); }
	static Vector4 PLUS_MAX() { return Vector4( std::numeric_limits< Float >::max() ); }
	static Vector4 MINUS_MAX() { return -Vector4( std::numeric_limits< Float >::max() ); }
	static Vector4 PLUS_INF() { return Vector4( std::numeric_limits< Float >::infinity() ); }
	static Vector4 MINUS_INF() { return -Vector4( std::numeric_limits< Float >::infinity() ); }

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

	Vector4 operator-()
	{
		return Vector4( -X, -Y, -Z, -W );
	}

	Vector4 operator+( const Vector4& vec ) const
	{
		return Vector4( X + vec.X, Y + vec.Y, Z + vec.Z, W + vec.W );
	}

	Vector4 operator-( const Vector4& vec ) const
	{
		return Vector4( X - vec.X, Y - vec.Y, Z - vec.Z, W - vec.W );
	}

	Vector4 operator*( Float val ) const
	{
		return Vector4( X * val, Y * val, Z * val, W * val );
	}

	Vector4 operator/( Float val ) const
	{
		Float inv = 1.0f / val;

		return Vector4( X * inv, Y * inv, Z * inv, W * inv );
	}

	Float& operator[]( Uint32 index ) const
	{
		FORGE_ASSERT( index < 4 );

		Float* ptr = const_cast< Float* >( &X );
		return *( ptr + index );
	}

	void operator*=( Float val )
	{
		X *= val;
		Y *= val;
		Z *= val;
		W *= val;
	}

	void operator/=( Float val )
	{
		X /= val;
		Y /= val;
		Z /= val;
		W /= val;
	}

	void operator-=( const Vector4& vec )
	{
		X -= vec.X;
		Y -= vec.Y;
		Z -= vec.Z;
		W -= vec.W;
	}

	void operator+=( const Vector4& vec )
	{
		X += vec.X;
		Y += vec.Y;
		Z += vec.Z;
		W += vec.W;
	}

	Bool operator==( const Vector4& vec ) const
	{
		return X == vec.X && Y == vec.Y && vec.Z == Z && vec.W == vec.W;
	}

	Bool operator!=( const Vector4& vec ) const
	{
		return X != vec.X || Y != vec.Y || vec.Z != Z || vec.W != W;
	}

	Bool IsZero() const { return X == 0.0f && Y == 0.0f && Z == 0.0f && W == 0.0f; }
	Bool IsAlmostZero( Float eps = std::numeric_limits< Float >::epsilon() ) const;

	Float Mag3() const
	{
		return std::sqrt( X * X + Y * Y + Z * Z );
	}

	Float SquareMag3() const
	{
		return X * X + Y * Y + Z * Z; 
	}

	Vector4 Normalized3() const
	{
		Float mag = Mag3();
		return Vector4( X / mag, Y / mag, Z / mag, W / mag );
	}

	Float Normalize3()
	{
		Float mag = Mag3();

		X /= mag;
		Y /= mag;
		Z /= mag;
		W /= mag;

		return mag;
	}

	Float Dot3( const Vector4& vec ) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z;
	}

	Float Mag4() const
	{
		return std::sqrt( X * X + Y * Y + Z * Z + W * W );
	}
	Float SquareMag4() const
	{
		return X * X + Y * Y + Z * Z + W * W; 
	}

	Vector4 Normalized4() const
	{
		Float mag = Mag4();
		return Vector4( X / mag, Y / mag, Z / mag, W / mag );
	}

	Float Normalize4()
	{
		Float mag = Mag4();

		X /= mag;
		Y /= mag;
		Z /= mag;
		W /= mag;

		return mag;
	}

	Float Dot4( const Vector4& vec ) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z + W * vec.W;
	}

	Bool IsOk() const
	{
		return abs( X ) != std::numeric_limits< Float >::infinity()
			&& abs( Y ) != std::numeric_limits< Float >::infinity()
			&& abs( Z ) != std::numeric_limits< Float >::infinity()
			&& abs( W ) != std::numeric_limits< Float >::infinity();
	}

	Bool IsAlmostEqual( const Vector4& vec, Float eps = std::numeric_limits< Float >::epsilon() ) const
	{
		return ( *this - vec ).IsAlmostZero( eps );
	}

	const Vector3& AsVector3() const
	{
		return vec3;
	}

	const Float* AsArray() const
	{
		return &X;
	}

#ifndef FORGE_FINAL
	std::string ToDebugString( Uint32 precision = 2u ) const;
#endif
};

