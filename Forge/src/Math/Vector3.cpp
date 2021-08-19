#include "Fpch.h"
#include "Vector3.h"

Vector3 Vector3::PLUS_MAX()
{
	return Vector3( std::numeric_limits< Float >::max() );
}

Vector3 Vector3::MINUS_MAX()
{
	return -Vector3( std::numeric_limits< Float >::max() );
}

Vector3 Vector3::PLUS_INF()
{
	return Vector3( std::numeric_limits< Float >::infinity() );
}

Vector3 Vector3::MINUS_INF()
{
	return -Vector3( std::numeric_limits< Float >::infinity() );
}

Vector3::Vector3( const Vector4& vec )
	: X( vec.X )
	, Y( vec.Y )
	, Z( vec.Z )
{}

Bool Vector3::IsAlmostZero( Float eps ) const
{
	return Math::IsAlmostZero( X, eps ) && Math::IsAlmostZero( Y, eps ) && Math::IsAlmostZero( Z, eps );
}

std::string Vector3::ToDebugString( Uint32 precision ) const
{	
	std::string format = forge::String::Printf( "[%s%u%s, %s%u%s, %s%u%s]", "%.", precision, "f", "%.", precision, "f", "%.", precision, "f" );
	return forge::String::Printf( format.c_str(), X, Y, Z );
}
