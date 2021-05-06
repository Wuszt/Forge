#include "Fpch.h"
#include "Vector3.h"
#include "Vector4.h"

Vector3::Vector3( const Vector4& vec )
	: X( vec.X )
	, Y( vec.Y )
	, Z( vec.Z )
{}
