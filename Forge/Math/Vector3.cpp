#include "Fpch.h"
#include "Vector3.h"

Vector3::Vector3( const Vector4& vec )
	: X( vec.X )
	, Y( vec.Y )
	, Z( vec.Z )
{}

Bool Vector3::IsAlmostZero( Float eps ) const
{
	return Math::IsAlmostZero( X, eps ) && Math::IsAlmostZero( Y, eps ) && Math::IsAlmostZero( Z, eps );
}
