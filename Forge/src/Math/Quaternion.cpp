#include "Fpch.h"
#include "Quaternion.h"

Quaternion Quaternion::GetRotationBetweenVectors( const Vector3& first, const Vector3& second, const Vector3& up /*= Vector3::EZ()*/ )
{
	const Float kcos = first.Dot( second );
	const Float k = sqrt( first.SquareMag() * second.SquareMag() );

	if( Math::IsAlmostZero( kcos / k + 1.0f ) )
	{
		return Quaternion( up.X, up.Y, up.Z, 0.0f );
	}

	Vector3 perpendicular = first.Cross( second );
	return Quaternion( perpendicular.X, perpendicular.Y, perpendicular.Z, kcos + k ).Normalized();
}

Quaternion Quaternion::Lerp( const Quaternion& from, const Quaternion& to, Float t )
{
	const Float dot = from.vec4.Dot4( to.vec4 );
	const Float sign = Math::Sign( dot );

	return Math::Lerp( from, to * sign, t );
}

Quaternion Quaternion::Slerp( const Quaternion& from, const Quaternion& to, Float t )
{
	t = Math::Clamp( 0.0f, 1.0f, t );

	const Float cosHalfTheta = from.vec4.Dot4( to.vec4 );

	const Float sign = Math::Sign( cosHalfTheta );

	if( Math::Abs( cosHalfTheta ) >= 1.0f )
	{
		return from;
	}

	const Float halfTheta = Math::Acos( sign * cosHalfTheta );
	const Float sin = Math::Sin( halfTheta );

	if( Math::IsAlmostZero( sin, 0.01f ) )
	{
		return from;
	}

	FORGE_ASSERT( !Math::IsAlmostZero( sin, 0.001f ) );

	const Float wa = Math::Sin( ( 1.0f - t ) * halfTheta ) / sin;
	const Float wb = sign * Math::Sin( t * halfTheta ) / sin;

	Quaternion result( from.vec4 * wa + to.vec4 * wb );

	return result.Normalized();
}

Vector4 Quaternion::Transform( const Vector4& vec ) const
{
	FORGE_ASSERT( Math::IsAlmostZero( 1.0f - vec4.SquareMag4() ) );

	const Float vMult = 2.0f*( i*vec.X + j * vec.Y + k * vec.Z );
	const Float crossMult = 2.0f*r;
	const Float pMult = crossMult * r - 1.0f;

	return Vector4( pMult*vec.X + vMult * i + crossMult * ( j*vec.Z - k * vec.Y ),
		pMult*vec.Y + vMult * j + crossMult * ( k*vec.X - i * vec.Z ),
		pMult*vec.Z + vMult * k + crossMult * ( i*vec.Y - j * vec.X ), vec.W );
}

void Quaternion::SetAxisAngle( const Vector4& vec, Float angle )
{
	Float halfAngle = angle * 0.5f;

	r = Math::Cos( halfAngle );
	vec3 = vec * Math::Sin( halfAngle );

	Normalize();
}

Quaternion::Quaternion( Float xRotation, Float yRotation, Float zRotation )
{
	//yaw-pitch-roll order -> Z-X-Y order
	xRotation *= 0.5f;
	yRotation *= 0.5f;
	zRotation *= 0.5f;

	Float c1 = Math::Cos( xRotation );
	Float s1 = Math::Sin( xRotation );

	Float c2 = Math::Cos( yRotation );
	Float s2 = Math::Sin( yRotation );

	Float c3 = Math::Cos( zRotation );
	Float s3 = Math::Sin( zRotation );

	i = s1 * c2 * c3 - c1 * s2 * s3;
	j = c1 * s2 * c3 + s1 * c2 * s3;
	k = c1 * c2 * s3 + s1 * s2 * c3;
	r = c1 * c2 * c3 - s1 * s2 * s3;
}

Quaternion::Quaternion( Vector4 vec )
{
	vec4 = std::move( vec );
}

Quaternion::Quaternion( const Vector3& eulerAngles )
	: Quaternion( eulerAngles.X, eulerAngles.Y, eulerAngles.Z )
{}

Quaternion::~Quaternion() = default;

Quaternion Quaternion::operator*( const Quaternion & q ) const
{
	Quaternion result( Vector4( q.vec3 * r + vec3 * q.r + vec3.Cross( q.vec3 ), r * q.r - vec3.Dot( q.vec3 ) ) );
	result.Normalize();
	return result;
}

Quaternion Quaternion::operator*( Float val ) const
{
	return Quaternion( vec4 * val );
}

Quaternion Quaternion::operator/( Float val ) const
{
	return Quaternion( vec4 / val );
}

Quaternion Quaternion::operator+( const Quaternion& q ) const
{
	return Quaternion( vec4 + q.vec4 );
}

Quaternion Quaternion::operator-( const Quaternion& q ) const
{
	return Quaternion( vec4 - q.vec4 );
}

Bool Quaternion::operator==( const Quaternion& q ) const
{
	return vec4 == q.vec4;
}

Bool Quaternion::operator!=( const Quaternion& q ) const
{
	return vec4 != q.vec4;
}

Quaternion Quaternion::Inverted() const
{
	return Quaternion( -vec3.X, -vec3.Y, -vec3.Z, r );
}

void Quaternion::Invert()
{
	vec3 = -vec3;
}

Float Quaternion::Normalize()
{
	return vec4.Normalize4();
}

Quaternion Quaternion::Normalized() const
{
	return Quaternion( vec4.Normalized4() );
}

Float Quaternion::Dot( const Quaternion& q ) const
{
	return vec4.Dot4( q.vec4 );
}

Float Quaternion::Mag() const
{
	return vec4.Mag4();
}

Float Quaternion::SquareMag() const
{
	return vec4.SquareMag4();
}

Bool Quaternion::IsAlmostEqual( const Quaternion& q, Float eps ) const
{
	return vec4.IsAlmostEqual( q.vec4, eps );
}

Vector3 Quaternion::ToEulerAngles() const
{
	Vector3 euler;
	Float y2 = Math::Clamp( -1.0f, 1.0f, 2.0f * ( j * k + r * i ) );
	euler.X = std::asin( y2 );
	if ( Math::Abs( y2 ) < 1.0f )
	{
		euler.Y = Math::Atan2( 2.0f * ( r * j - i * k ), 1.0f - 2.0f * ( i * i + j * j ) );
		euler.Z = Math::Atan2( 2.0f * ( r * k - i * j ), 1.0f - 2.0f * ( i * i + k * k ) );
	}
	else
	{
		euler.Z = Math::Atan2( 2.0f * ( i * j + r * k ), 1.0f - 2.0f * ( j * j + k * k ) );
	}

	return euler;
}

Vector3 Quaternion::GetXAxis3() const
{
	return *this * Vector3::EX();
}

Vector3 Quaternion::GetYAxis3() const
{
	return *this * Vector3::EY();
}

Vector3 Quaternion::GetZAxis3() const
{
	return *this * Vector3::EZ();
}

Vector4 Quaternion::GetXAxis4() const
{
	return *this * Vector4::EX();
}

Vector4 Quaternion::GetYAxis4() const
{
	return *this * Vector4::EY();
}

Vector4 Quaternion::GetZAxis4() const
{
	return *this * Vector4::EZ();
}
