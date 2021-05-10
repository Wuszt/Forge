#include "Fpch.h"
#include "Vector3.h"

Vector3::Vector3( const Vector4& vec )
	: X( vec.X )
	, Y( vec.Y )
	, Z( vec.Z )
{}

Bool Vector3::AlmostEqual( const Vector3 & vec, Float eps ) const
{
    return Math::IsAlmostZero( X - vec.X, eps )
    && Math::IsAlmostZero( Y - vec.Y, eps )
    && Math::IsAlmostZero( Z - vec.Z, eps );
}

Vector3 Vector3::Cross( const Vector3 & vec ) const
{
    return Vector3(
        Y * vec.Z - Z * vec.Y,
        Z * vec.X - X * vec.Z,
        X * vec.Y - Y * vec.X );
}
