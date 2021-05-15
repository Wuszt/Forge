#pragma once

// ROW MAJOR!!!
struct Matrix
{
	FORGE_INLINE static Matrix IDENTITY() { return Matrix( Vector4::EX(), Vector4::EY(), Vector4::EZ(), Vector4::EW() ); }

	static Matrix Mul( const Matrix& m0, const Matrix& m1 )
	{
		return m0 * m1;
	}

	template< class T0, class T1, class... Args >
	static Matrix Mul( const Matrix& m0, const Matrix& m1, Args... matrices )
	{
		return Mul( Mul( m0, m1 ), matrices );
	}

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

	Matrix( const Quaternion& q );

	Matrix( Float x0, Float x1, Float x2, Float x3,
		Float y0, Float y1, Float y2, Float y3,
		Float z0, Float z1, Float z2, Float z3,
		Float w0, Float w1, Float w2, Float w3 )
		: X( x0, x1, x2, x3 )
		, Y( y0, y1, y2, y3 )
		, Z( z0, z1, z2, z3 )
		, W( w0, w1, w2, w3 )
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

	void Transpose();

	Matrix Transposed() const
	{
		return Matrix( X[ 0 ], Y[ 0 ], Z[ 0 ], W[ 0 ],
					   X[ 1 ], Y[ 1 ], Z[ 1 ], W[ 1 ],
					   X[ 2 ], Y[ 2 ], Z[ 2 ], W[ 2 ],
					   X[ 3 ], Y[ 3 ], Z[ 3 ], W[ 3 ] );
	}

	void OrthonormInvert();

	void AffineInvert();

	FORGE_INLINE Quaternion ToQuat() const
	{
		Quaternion result;

		Float trace = X[ 0 ] + Y[ 1 ] + Z[ 2 ];

		if( trace > 0 )
		{
			Float s = std::sqrtf( trace + 1.0f ) * 2.0f;
			Float invS = 1.0f / s;
			result.r = 0.25f * s;

			result.i = ( Y[ 2 ] - Z[ 1 ] ) * invS;
			result.j = ( Z[ 0 ] - X[ 2 ] ) * invS;
			result.k = ( X[ 1 ] - Y[ 0 ] ) * invS;
		}
		else if( X[ 0 ] > Y[ 1 ] && X[ 0 ] > Z[ 2 ] )
		{
			Float s = std::sqrtf( X[ 0 ] - Y[ 1 ] - Z[ 2 ] + 1.0f ) * 2.0f;
			Float invS = 1.0f / s;

			result.i = 0.25f * s;

			result.r = ( Y[ 2 ] - Z[ 1 ] ) * invS;
			result.j = ( Y[ 0 ] + X[ 1 ] ) * invS;
			result.k = ( Z[ 0 ] + X[ 2 ] ) * invS;
		}
		else if( Y[ 1 ] > Z[ 2 ] )
		{
			Float s = std::sqrtf( Y[ 1 ] - X[ 0 ] - Z[ 2 ] + 1.0f ) * 2.0f;
			Float invS = 1.0f / s;

			result.j = 0.25f * s;

			result.r = ( Z[ 0 ] - X[ 2 ] ) * invS;
			result.i = ( Y[ 0 ] + X[ 1 ] ) * invS;
			result.k = ( Z[ 1 ] + Y[ 2 ] ) * invS;
		}
		else
		{
			Float s = std::sqrtf( Z[ 2 ] - X[ 0 ] - Y[ 1 ] + 1.0f ) * 2.0f;
			Float invS = 1.0f / s;

			result.k = 0.25f * s;

			result.r = ( X[ 1 ] - Y[ 0 ] ) * invS;
			result.i = ( Z[ 0 ] + X[ 2 ] ) * invS;
			result.j = ( Z[ 1 ] + Y[ 2 ] ) * invS;
		}

		return result;
	}

	FORGE_INLINE void SetScale( Float scale )
	{
		X.X = scale;
		Y.Y = scale;
		Z.Z = scale;
	}

	FORGE_INLINE void SetScale( const Vector3& scale )
	{
		X.X = scale.X;
		Y.Y = scale.Y;
		Z.Z = scale.Z;
	}

	FORGE_INLINE Bool operator==( const Matrix& m ) const
	{
		return X == m.X && Y == m.Y && Z == m.Z && W == m.W;
	}

	FORGE_INLINE Matrix operator*( const Matrix& m ) const
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

