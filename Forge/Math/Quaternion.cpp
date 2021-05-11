#include "Fpch.h"
#include "Quaternion.h"

Vector4 Quaternion::Transform( const Vector4& vec ) const
{
	FORGE_ASSERT( Math::IsAlmostZero( 1.0f - vec4.SquareMag4() ) );

	float vMult = 2.0f*( i*vec.X + j * vec.Y + k * vec.Z );
	float crossMult = 2.0f*r;
	float pMult = crossMult * r - 1.0f;

	return Vector3( pMult*vec.X + vMult * i + crossMult * ( j*vec.Z - k * vec.Y ),
		pMult*vec.Y + vMult * j + crossMult * ( k*vec.X - i * vec.Z ),
		pMult*vec.Z + vMult * k + crossMult * ( i*vec.Y - j * vec.X ) );

}

void Quaternion::SetAxisAngle( const Vector4& vec, Float angle )
{
	Float halfAngle = angle * 0.5f;

	r = Math::Cos( halfAngle );
	vec3 = vec * Math::Sin( halfAngle );

	vec4.Normalize4();
}

Quaternion::Quaternion( Float xRotation, Float yRotation, Float zRotation )
{
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

}

Quaternion::Quaternion( const Vector4& vec )
{
	vec4 = vec;
}

Quaternion::~Quaternion() = default;

Quaternion Quaternion::operator*( const Quaternion & q ) const
{
	return Quaternion( Vector4( q.vec3 * r + vec3 * q.r + vec3.Cross( q.vec3 ), r * q.r - vec3.Dot( q.vec3 ) ) );
}

Quaternion Quaternion::Conjugate() const
{
	Quaternion copy( vec4 );
	copy.r = -copy.r;
	return copy;
}

Float Quaternion::Normalize()
{
	return vec4.Normalize4();
}
