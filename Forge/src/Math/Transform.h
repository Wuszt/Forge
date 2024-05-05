#pragma once
class Transform
{
	RTTI_DECLARE_CLASS( Transform );

public:
	static Transform IDENTITY() { return Transform(); }

	Transform();
	Transform( const Vector3& pos );
	Transform( const Vector4& pos );
	Transform( const Quaternion& q );
	Transform( const Vector4& pos, const Quaternion& q );
	Transform( const Matrix& matrix );

	~Transform();

	void SetPosition( const Vector3& position )
	{
		m_position = position;
	}

	void SetOrientation( const Quaternion& orientation )
	{
		m_orientation = orientation;
	}

	const Vector4& GetPosition() const
	{
		return m_position;
	}

	const Vector3& GetPosition3() const
	{
		return m_position.AsVector3();
	}

	const Quaternion& GetOrientation() const
	{
		return m_orientation;
	}

	Matrix ToMatrix() const;

	Vector4 TransformVector( const Vector4& vec ) const
	{
		return ToMatrix().TransformVector( vec );
	}

	Vector4 TransformVectorWithW( const Vector3& vec ) const
	{
		return ToMatrix().TransformVectorWithW( vec );
	}

	Vector4 TransformPoint( const Vector4& point ) const
	{
		return ToMatrix().TransformPoint( point );
	}

	Vector4 GetForward() const
	{
		return m_orientation * Vector4::EY();
	}

	Vector4 GetRight() const
	{
		return m_orientation * Vector4::EX();
	}

	Vector4 GetUp() const
	{
		return m_orientation * Vector4::EZ();
	}

	Vector3 operator*( const Vector3& vec ) const
	{
		return ToMatrix().TransformVectorWithW( vec );
	}

	Vector4 operator*( const Vector4& vec ) const
	{
		return ToMatrix().TransformVectorWithW( vec );
	}

	Transform operator*( const Transform& transform ) const
	{
		return Transform( ToMatrix() * transform.ToMatrix() );
	}

	void SetIdentity()
	{
		*this = Transform();
	}

	void Invert()
	{
		m_position = -m_position;
		m_orientation.Invert();
	}

	Transform Inverted() const
	{
		return Transform( Vector4( -m_position.AsVector3(), m_position.W ), m_orientation.Inverted() );
	}

private:
	Vector4 m_position = Vector4::EW();
	Quaternion m_orientation = Quaternion::IDENTITY();
};

