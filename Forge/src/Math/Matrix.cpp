#include "Fpch.h"
#include "Matrix.h"

Matrix::Matrix( const Quaternion& q )
{
	FORGE_ASSERT( Math::IsAlmostZero( 1.0f - q.SquareMag() ) );

	X = { 1.0f - 2.0f * q.j * q.j - 2.0f * q.k * q.k, 2.0f * q.i * q.j + 2.0f * q.r * q.k, 2.0f * q.i * q.k - 2.0f * q.r * q.j, 0.0f };
	Y = { 2.0f * q.i * q.j - 2.0f * q.r * q.k, 1.0f - 2.0f * q.i * q.i - 2.0f * q.k * q.k, 2.0f * q.j * q.k + 2.0f * q.r * q.i, 0.0f };
	Z = { 2.0f * q.i * q.k + 2.0f * q.r * q.j, 2.0f * q.j * q.k - 2.0f * q.r * q.i, 1.0f - 2.0f * q.i * q.i - 2.0f * q.j * q.j, 0.0f };
	W = Vector4::EW();
}

void Matrix::SetRotationX( Float rad )
{
	Float sin = Math::Sin( rad );
	Float cos = Math::Cos( rad );

	X.X = 1.0f;
	X.Y = 0.0f;
	X.Z = 0.0f;

	Y.X = 0.0f;
	Y.Y = cos;
	Y.Z = sin;

	Z.X = 0.0f;
	Z.Y = -sin;
	Z.Z = cos;
}

void Matrix::SetRotationY( Float rad )
{
	Float sin = Math::Sin( rad );
	Float cos = Math::Cos( rad );

	X.X = 0.0f;
	X.X = cos;
	X.Z = -sin;

	Y.X = 0.0f;
	Y.Y = 1.0f;
	Y.Z = 0.0f;

	Z.X = sin;
	Z.Y = 0.0f;
	Z.Z = cos;
}

void Matrix::SetRotationZ( Float rad )
{
	Float sin = Math::Sin( rad );
	Float cos = Math::Cos( rad );

	X.X = cos;
	X.Y = sin;
	X.Z = 0.0f;

	Y.X = -sin;
	Y.Y = cos;
	Y.Z = 0.0f;

	Z.X = 0.0f;
	Z.Y = 0.0f;
	Z.Z = 1.0f;
}

void Matrix::Transpose()
{
	Math::Swap( X[ 1 ], Y[ 0 ] );
	Math::Swap( X[ 2 ], Z[ 0 ] );
	Math::Swap( X[ 3 ], W[ 0 ] );
	Math::Swap( Y[ 2 ], Z[ 1 ] );
	Math::Swap( Y[ 3 ], W[ 1 ] );
	Math::Swap( Z[ 3 ], W[ 2 ] );
}

void Matrix::OrthonormInvert()
{
	Vector4 translation = W;
	SetTranslation( 0.0f, 0.0f, 0.0f );
	Transpose();

	translation = TransformPoint( -translation );
	W = translation;
}

void Matrix::AffineInvert()
{
	Matrix copy = *this;

	Float cofactor0 = copy.Y[ 1 ] * copy.Z[ 2 ] - copy.Y[ 2 ] * copy.Z[ 1 ];
	Float cofactor4 = copy.X[ 1 ] * copy.Z[ 2 ] - copy.X[ 2 ] * copy.Z[ 1 ];
	Float cofactor8 = copy.X[ 1 ] * copy.Y[ 2 ] - copy.X[ 2 ] * copy.Y[ 1 ];

	Float det = copy.X[ 0 ] * cofactor0 + copy.Y[ 0 ] * cofactor4 + copy.Z[ 0 ] * cofactor8;

	FORGE_ASSERT( !Math::IsAlmostZero( det ) );

	Float invDet = 1.0f / det;

	X[ 0 ] = invDet * cofactor0;
	X[ 1 ] = invDet * cofactor4;
	X[ 2 ] = invDet * cofactor8;

	Y[ 0 ] = -invDet * ( copy.Y[ 0 ] * copy.Z[ 2 ] - copy.Y[ 2 ] * copy.Z[ 0 ] );
	Y[ 1 ] = invDet * ( copy.X[ 0 ] * copy.Z[ 2 ] - copy.X[ 2 ] * copy.Z[ 0 ] );
	Y[ 2 ] = -invDet * ( copy.X[ 0 ] * copy.Y[ 2 ] - copy.Y[ 0 ] * copy.X[ 2 ] );

	Z[ 0 ] = invDet * ( copy.Y[ 0 ] * copy.Z[ 1 ] - copy.Z[ 0 ] * copy.Y[ 1 ] );
	Z[ 1 ] = -invDet * ( copy.X[ 0 ] * copy.Z[ 1 ] - copy.Z[ 0 ] * copy.X[ 1 ] );
	Z[ 2 ] = invDet * ( copy.X[ 0 ] * copy.Y[ 1 ] - copy.Y[ 0 ] * copy.X[ 1 ] );

	W[ 0 ] = -( copy.W[ 0 ] * X[ 0 ] + copy.W[ 1 ] * Y[ 0 ] + copy.W[ 2 ] * Z[ 0 ] );
	W[ 1 ] = -( copy.W[ 0 ] * X[ 1 ] + copy.W[ 1 ] * Y[ 1 ] + copy.W[ 2 ] * Z[ 1 ] );
	W[ 2 ] = -( copy.W[ 0 ] * X[ 2 ] + copy.W[ 1 ] * Y[ 2 ] + copy.W[ 2 ] * Z[ 2 ] );
}
