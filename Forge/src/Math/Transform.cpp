#include "Fpch.h"
#include "Transform.h"

Transform::Transform( const Vector4& pos )
	: m_position( pos.X, pos.Y, pos.Z, 1.0f )
{}

Transform::Transform( const Vector3& pos )
	: m_position( pos )
{}

Transform::Transform( const Quaternion& q )
	: m_orientation( q.Normalized() )
{}

Transform::Transform()
{}

Transform::Transform( const Vector4& pos, const Quaternion& q )
	: m_position( pos.X, pos.Y, pos.Z, 1.0f )
	, m_orientation( q.Normalized() )
{}

Transform::Transform( const Matrix& matrix )
	: m_orientation( matrix.ToQuat() )
	, m_position( matrix.GetTranslation() )
{}

Transform::~Transform()
{}

Matrix Transform::ToMatrix() const
{
	Matrix result( m_orientation );
	result.SetTranslation( GetPosition3() );

	return result;
}
