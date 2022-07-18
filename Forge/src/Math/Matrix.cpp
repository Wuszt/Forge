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

	X.X = cos;
	X.Y = 0.0f;
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

Matrix Matrix::OrthonormInverted() const
{
	Matrix copy = *this;
	copy.OrthonormInvert();

	return copy;
}

void Matrix::Decompose( Vector3& scale, Quaternion& rotation, Vector3& translation )
{
	translation = W;

	scale = { X.Mag3(), Y.Mag3(), Z.Mag3() };

	Matrix m = *this;
	m.X /= scale.X;
	m.Y /= scale.Y;
	m.Z /= scale.Z;

	rotation = m.GetRotation();
}

Vector3 Matrix::ToEulerAngles() const
{
	Vector3 euler;
	euler.X = std::asin( Math::Clamp( -1.0f, 1.0f, Y[ 2 ] ) );
	if ( Math::Abs( Y[ 2 ] ) < 1.0f )
	{
		euler.Y = std::atan2( -X[ 2 ], Z[ 2 ] );
		euler.Z = std::atan2( -Y[ 0 ], Y[ 1 ] );
	}
	else
	{
		euler.Z = std::atan2( X[ 1 ], X[ 0 ] );
	}

	return euler;
}

void AffineInversion( Matrix copy, Matrix& destination )
{
	Float cofactor0 = copy.Y[ 1 ] * copy.Z[ 2 ] - copy.Y[ 2 ] * copy.Z[ 1 ];
	Float cofactor4 = copy.X[ 1 ] * copy.Z[ 2 ] - copy.X[ 2 ] * copy.Z[ 1 ];
	Float cofactor8 = copy.X[ 1 ] * copy.Y[ 2 ] - copy.X[ 2 ] * copy.Y[ 1 ];

	Float det = copy.X[ 0 ] * cofactor0 + copy.Y[ 0 ] * cofactor4 + copy.Z[ 0 ] * cofactor8;

	FORGE_ASSERT( !Math::IsAlmostZero( det ) );

	Float invDet = 1.0f / det;

	destination.X[ 0 ] = invDet * cofactor0;
	destination.X[ 1 ] = invDet * cofactor4;
	destination.X[ 2 ] = invDet * cofactor8;

	destination.Y[ 0 ] = -invDet * ( copy.Y[ 0 ] * copy.Z[ 2 ] - copy.Y[ 2 ] * copy.Z[ 0 ] );
	destination.Y[ 1 ] = invDet * ( copy.X[ 0 ] * copy.Z[ 2 ] - copy.X[ 2 ] * copy.Z[ 0 ] );
	destination.Y[ 2 ] = -invDet * ( copy.X[ 0 ] * copy.Y[ 2 ] - copy.Y[ 0 ] * copy.X[ 2 ] );

	destination.Z[ 0 ] = invDet * ( copy.Y[ 0 ] * copy.Z[ 1 ] - copy.Z[ 0 ] * copy.Y[ 1 ] );
	destination.Z[ 1 ] = -invDet * ( copy.X[ 0 ] * copy.Z[ 1 ] - copy.Z[ 0 ] * copy.X[ 1 ] );
	destination.Z[ 2 ] = invDet * ( copy.X[ 0 ] * copy.Y[ 1 ] - copy.Y[ 0 ] * copy.X[ 1 ] );

	destination.W[ 0 ] = -( copy.W[ 0 ] * destination.X[ 0 ] + copy.W[ 1 ] * destination.Y[ 0 ] + copy.W[ 2 ] * destination.Z[ 0 ] );
	destination.W[ 1 ] = -( copy.W[ 0 ] * destination.X[ 1 ] + copy.W[ 1 ] * destination.Y[ 1 ] + copy.W[ 2 ] * destination.Z[ 1 ] );
	destination.W[ 2 ] = -( copy.W[ 0 ] * destination.X[ 2 ] + copy.W[ 1 ] * destination.Y[ 2 ] + copy.W[ 2 ] * destination.Z[ 2 ] );
}

void Matrix::AffineInvert()
{
	AffineInversion( *this, *this );
}

Matrix Matrix::AffineInverted() const
{
	Matrix result;
	AffineInversion( *this, result );

	return result;
}
