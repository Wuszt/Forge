#include "Fpch.h"
#include "Vector4.h"

Vector4::Vector4( const Vector3& vec )
	: X( vec.X )
	, Y( vec.Y )
	, Z( vec.Z )
	, W( 1.0f )
{}

Vector4::Vector4( const Vector3& vec, Float w )
	: X( vec.X )
	, Y( vec.Y )
	, Z( vec.Z )
	, W( w )
{}

Bool Vector4::IsAlmostZero( Float eps ) const
{
	return Math::IsAlmostZero( X, eps ) && Math::IsAlmostZero( Y, eps ) && Math::IsAlmostZero( Z, eps ) && Math::IsAlmostZero( W, eps );
}

#ifdef FORGE_DEBUG
std::string Vector4::ToDebugString( Uint32 precision ) const
{
	std::string format = forge::String::Printf( "[%s%u%s, %s%u%s, %s%u%s, %s%u%s]", "%.", precision, "f", "%.", precision, "f", "%.", precision, "f", "%.", precision, "f" );
	return forge::String::Printf( format.c_str(), X, Y, Z, W );
}
#endif
