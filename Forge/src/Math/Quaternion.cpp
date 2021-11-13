#include "Fpch.h"
#include "Quaternion.h"

Vector4 Quaternion::Transform( const Vector4& vec ) const
{
	FORGE_ASSERT( Math::IsAlmostZero( 1.0f - vec4.SquareMag4() ) );

	float vMult = 2.0f*( i*vec.X + j * vec.Y + k * vec.Z );
	float crossMult = 2.0f*r;
	float pMult = crossMult * r - 1.0f;

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

	Float cX = Math::Cos( xRotation );
	Float sX = Math::Sin( xRotation );

	Float cY = Math::Cos( yRotation );
	Float sY = Math::Sin( yRotation );

	Float cZ = Math::Cos( zRotation );
	Float sZ = Math::Sin( zRotation );

	r = cY * cX * cZ - sY * sX * sZ;
	j = sY * cX * cZ - cY * sX * sZ;
	i = cY * sX * cZ + sY * cX * sZ;
	k = cY * cX * sZ + sY * sX * cY;

	Normalize(); //todo: is it necessary?
}

Quaternion::Quaternion( const Vector4& vec )
{
	vec4 = vec;
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
