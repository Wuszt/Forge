#pragma once
class Transform
{
public:
	static Transform IDENTITY() { return Transform(); }

	Transform();
	Transform( const Vector3& pos );
	Transform( const Vector4& pos );
	Transform( const Quaternion& q );
	Transform( const Vector4& pos, const Quaternion& q );
	Transform( const Matrix& matrix );

	~Transform();

	FORGE_INLINE void SetPosition( const Vector3& position )
	{
		m_position = position;
	}

	FORGE_INLINE void SetOrientation( const Quaternion& orientation )
	{
		m_orientation = orientation;
	}

	FORGE_INLINE const Vector4& GetPosition() const
	{
		return m_position;
	}

	FORGE_INLINE const Vector3& GetPosition3() const
	{
		return m_position.AsVector3();
	}

	FORGE_INLINE const Quaternion& GetOrientation() const
	{
		return m_orientation;
	}

	Matrix ToMatrix() const;

	FORGE_INLINE Vector4 TransformVector( const Vector4& vec ) const
	{
		return ToMatrix().TransformVector( vec );
	}

	FORGE_INLINE Vector4 TransformVectorWithW( const Vector3& vec ) const
	{
		return ToMatrix().TransformVectorWithW( vec );
	}

	FORGE_INLINE Vector4 TransformPoint( const Vector4& point ) const
	{
		return ToMatrix().TransformPoint( point );
	}

	FORGE_INLINE Vector4 GetForward() const
	{
		return m_orientation * Vector4::EY();
	}

	FORGE_INLINE Vector4 GetRight() const
	{
		return m_orientation * Vector4::EX();
	}

	FORGE_INLINE Vector4 GetUp() const
	{
		return m_orientation * Vector4::EZ();
	}

	FORGE_INLINE Vector3 operator*( const Vector3& vec ) const
	{
		return ToMatrix().TransformVectorWithW( vec );
	}

	FORGE_INLINE Vector4 operator*( const Vector4& vec ) const
	{
		return ToMatrix().TransformVectorWithW( vec );
	}

	FORGE_INLINE Transform operator*( const Transform& transform ) const
	{
		return Transform( ToMatrix() * transform.ToMatrix() );
	}

	FORGE_INLINE void SetIdentity()
	{
		*this = Transform();
	}

private:
	Vector4 m_position = Vector4::EW();
	Quaternion m_orientation = Quaternion::IDENTITY();
};

