#pragma once
#include "Fpch.h"

#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

// ROW MAJOR!!!
class Matrix
{
public:

	FORGE_INLINE static Matrix IDENTITY()
	{
		return Matrix( Vector4::EX(), Vector4::EY(), Vector4::EZ(), Vector4::EW() );
	}

	Matrix() {}

	Matrix( const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w )
		: X( x )
		, Y( y )
		, Z( z )
		, W( w )
	{}

	~Matrix();

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

	FORGE_INLINE Vector4 TransformPoint( const Vector4& vec )
	{
		Vector4 result;

		result.X = vec.X * arr[ 0 ] + vec.Y * arr[ 4 ] + vec.Z * arr[ 8 ] + arr[ 12 ];
		result.Y = vec.X * arr[ 1 ] + vec.Y * arr[ 5 ] + vec.Z * arr[ 9 ] + arr[ 13 ];
		result.Z = vec.X * arr[ 2 ] + vec.Y * arr[ 6 ] + vec.Z * arr[ 10 ] + arr[ 14 ];
		result.W = vec.X * arr[ 3 ] + vec.Y * arr[ 7 ] + vec.Z * arr[ 11 ] + arr[ 15 ];

		return result;
	}

	FORGE_INLINE Vector4 TransformVector( const Vector4& vec )
	{
		Vector4 result;

		result.X = vec.X * arr[ 0 ] + vec.Y * arr[ 4 ] + vec.Z * arr[ 8 ];
		result.Y = vec.X * arr[ 1 ] + vec.Y * arr[ 5 ] + vec.Z * arr[ 9 ];
		result.Z = vec.X * arr[ 2 ] + vec.Y * arr[ 6 ] + vec.Z * arr[ 10 ];
		result.W = vec.X * arr[ 3 ] + vec.Y * arr[ 7 ] + vec.Z * arr[ 11 ];

		return result;
	}
};

