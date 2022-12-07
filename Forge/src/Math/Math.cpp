#include "Fpch.h"
#include "Math.h"

template<>
Uint64 Math::CalculateHash( const Vector2& value )
{
	return Math::CombineHashes( Math::CalculateHash( value.X ), Math::CalculateHash( value.Y ) );
}

template<>
Uint64 Math::CalculateHash( const Vector3& value )
{
	return Math::CombineHashes( Math::CombineHashes( Math::CalculateHash( value.X ), Math::CalculateHash( value.Y ) ), Math::CalculateHash( value.Z ) );
}

template<>
Uint64 Math::CalculateHash( const Vector4& value )
{
	return Math::CombineHashes( Math::CombineHashes( Math::CombineHashes( Math::CalculateHash( value.X ), Math::CalculateHash( value.Y ) ), Math::CalculateHash( value.Z ) ), Math::CalculateHash( value.W ) );
}

template<>
Quaternion Math::Lerp( const Quaternion& from, const Quaternion& to, Float t )
{
	Quaternion result( Lerp( from.vec4, to.vec4, t ) );
	return result.Normalized();
}