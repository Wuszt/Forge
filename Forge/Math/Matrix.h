#pragma once

// ROW MAJOR!!!
struct Matrix
{
	FORGE_INLINE static Matrix IDENTITY() { return Matrix( Vector4::EX(), Vector4::EY(), Vector4::EZ(), Vector4::EW() ); }

	Matrix()
		: X( Vector4::EX() )
		, Y( Vector4::EY() )
		, Z( Vector4::EZ() )
		, W( Vector4::EW() )
	{}

	Matrix( const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w )
		: X( x )
		, Y( y )
		, Z( z )
		, W( w )
	{}

	~Matrix() {}

	FORGE_INLINE void SetIdentity()
	{
		*this = IDENTITY();
	}

	union
	{
		struct
		{
			Vector4 X;
			Vector4 Y;
			Vector4 Z;
			Vector4 W;
		};

		Float arr[16];
	};
	FORGE_INLINE Vector4& operator[]( Uint32 index ) const
	{
		Vector4* ptr = const_cast< Vector4* >( &X );
		return *( ptr + index );
	}

	FORGE_INLINE void SetTranslation( const Vector3& vec )
	{
		W = vec;
		W.W = 1.0f;
	}

	FORGE_INLINE void SetTranslation( Float x, Float y, Float z )
	{
		W = { x, y, z, 1.0f };
	}

	FORGE_INLINE Vector4 TransformPoint( const Vector4& vec ) const
	{
		Vector4 result;

		result.X = vec.X * arr[ 0 ] + vec.Y * arr[ 4 ] + vec.Z * arr[ 8 ] + arr[ 12 ];
		result.Y = vec.X * arr[ 1 ] + vec.Y * arr[ 5 ] + vec.Z * arr[ 9 ] + arr[ 13 ];
		result.Z = vec.X * arr[ 2 ] + vec.Y * arr[ 6 ] + vec.Z * arr[ 10 ] + arr[ 14 ];
		result.W = 1.0f;

		return result;
	}

	FORGE_INLINE Vector4 TransformVector( const Vector4& vec ) const
	{
		Vector4 result;

		result.X = vec.X * arr[ 0 ] + vec.Y * arr[ 4 ] + vec.Z * arr[ 8 ];
		result.Y = vec.X * arr[ 1 ] + vec.Y * arr[ 5 ] + vec.Z * arr[ 9 ];
		result.Z = vec.X * arr[ 2 ] + vec.Y * arr[ 6 ] + vec.Z * arr[ 10 ];
		result.W = 0.0f;

		return result;
	}

	void SetRotationX( Float rad );

	void SetRotationY( Float rad );

	void SetRotationZ( Float rad );

	FORGE_INLINE void SetScale( Float scale )
	{
		X.X = scale;
		Y.Y = scale;
		Z.Z = scale;
	}

	FORGE_INLINE void SetScale( const Vector4& scale )
	{
		X.X = scale.X;
		Y.Y = scale.Y;
		Z.Z = scale.Z;
	}

	FORGE_INLINE Matrix operator*( const Matrix& m )
	{
		Matrix result;

		result.X[ 0 ] = X[ 0 ] * m.arr[ 0 ] + X[ 1 ] * m.arr[ 4 ] + X[ 2 ] * m.arr[ 8 ]  + X[ 3 ] * m.arr[ 12 ];
		result.X[ 1 ] = X[ 0 ] * m.arr[ 1 ] + X[ 1 ] * m.arr[ 5 ] + X[ 2 ] * m.arr[ 9 ]  + X[ 3 ] * m.arr[ 13 ];
		result.X[ 2 ] = X[ 0 ] * m.arr[ 2 ] + X[ 1 ] * m.arr[ 6 ] + X[ 2 ] * m.arr[ 10 ] + X[ 3 ] * m.arr[ 14 ];
		result.X[ 3 ] = X[ 0 ] * m.arr[ 3 ] + X[ 1 ] * m.arr[ 7 ] + X[ 2 ] * m.arr[ 11 ] + X[ 3 ] * m.arr[ 15 ];

		result.Y[ 0 ] = Y[ 0 ] * m.arr[ 0 ] + Y[ 1 ] * m.arr[ 4 ] + Y[ 2 ] * m.arr[ 8 ]  + Y[ 3 ] * m.arr[ 12 ];
		result.Y[ 1 ] = Y[ 0 ] * m.arr[ 1 ] + Y[ 1 ] * m.arr[ 5 ] + Y[ 2 ] * m.arr[ 9 ]  + Y[ 3 ] * m.arr[ 13 ];
		result.Y[ 2 ] = Y[ 0 ] * m.arr[ 2 ] + Y[ 1 ] * m.arr[ 6 ] + Y[ 2 ] * m.arr[ 10 ] + Y[ 3 ] * m.arr[ 14 ];
		result.Y[ 3 ] = Y[ 0 ] * m.arr[ 3 ] + Y[ 1 ] * m.arr[ 7 ] + Y[ 2 ] * m.arr[ 11 ] + Y[ 3 ] * m.arr[ 15 ];
												  
		result.Z[ 0 ] = Z[ 0 ] * m.arr[ 0 ] + Z[ 1 ] * m.arr[ 4 ] + Z[ 2 ] * m.arr[ 8 ]  + Z[ 3 ] * m.arr[ 12 ];
		result.Z[ 1 ] = Z[ 0 ] * m.arr[ 1 ] + Z[ 1 ] * m.arr[ 5 ] + Z[ 2 ] * m.arr[ 9 ]  + Z[ 3 ] * m.arr[ 13 ];
		result.Z[ 2 ] = Z[ 0 ] * m.arr[ 2 ] + Z[ 1 ] * m.arr[ 6 ] + Z[ 2 ] * m.arr[ 10 ] + Z[ 3 ] * m.arr[ 14 ];
		result.Z[ 3 ] = Z[ 0 ] * m.arr[ 3 ] + Z[ 1 ] * m.arr[ 7 ] + Z[ 2 ] * m.arr[ 11 ] + Z[ 3 ] * m.arr[ 15 ];

		result.W[ 0 ] = W[ 0 ] * m.arr[ 0 ] + W[ 1 ] * m.arr[ 4 ] + W[ 2 ] * m.arr[ 8 ]  + W[ 3 ] * m.arr[ 12 ];
		result.W[ 1 ] = W[ 0 ] * m.arr[ 1 ] + W[ 1 ] * m.arr[ 5 ] + W[ 2 ] * m.arr[ 9 ]  + W[ 3 ] * m.arr[ 13 ];
		result.W[ 2 ] = W[ 0 ] * m.arr[ 2 ] + W[ 1 ] * m.arr[ 6 ] + W[ 2 ] * m.arr[ 10 ] + W[ 3 ] * m.arr[ 14 ];
		result.W[ 3 ] = W[ 0 ] * m.arr[ 3 ] + W[ 1 ] * m.arr[ 7 ] + W[ 2 ] * m.arr[ 11 ] + W[ 3 ] * m.arr[ 15 ];

		return result;
	}
};

