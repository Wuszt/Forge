#include "Fpch.h"
#include "Vector4.h"

Vector4::Vector4( const Vector3& vec )
	: X( vec.X )
	, Y( vec.Y )
	, Z( vec.Z )
	, W( 1.0f )
{}
