#pragma once

struct Quaternion
{   
	RTTI_DECLARE_STRUCT( Quaternion );

	union
	{
		Vector4 vec4;

		struct  
		{
			union
			{
				struct
				{
					Float i;
					Float j;
					Float k;
				};

				Vector3 vec3;
			};

			Float r;
		};
	};

	static Quaternion IDENTITY() { return Quaternion( 0.0f, 0.0f, 0.0f, 1.0f ); }
	static Quaternion I() { return Quaternion( 1.0f, 0.0f, 0.0f, 0.0f ); }
	static Quaternion J() { return Quaternion( 0.0f, 1.0f, 0.0f, 0.0f ); }
	static Quaternion K() { return Quaternion( 0.0f, 0.0f, 1.0f, 0.0f ); }

	static Quaternion GetRotationBetweenVectors( const Vector3& first, const Vector3& second, const Vector3& up = Vector3::EZ() );
	static Quaternion Lerp( const Quaternion& from, const Quaternion& to, float t );
	static Quaternion Slerp( const Quaternion& from, const Quaternion& to, float t );

	static Quaternion CreateFromDirection( const Vector3& direction, const Vector3& forward = Vector3::EY(), const Vector3& up = Vector3::EZ() )
	{
		return GetRotationBetweenVectors( forward, direction, up );
	}

	Vector4 Transform( const Vector4& vec ) const;
	Vector4 TransformInvert( const Vector4& vec ) const
	{
		return Inverted().Transform( vec );
	}

	void SetAxisAngle( const Vector4& vec, Float angle );

	Quaternion( Float i, Float j, Float k, Float r )
		: i( i )
		, j( j )
		, k( k )
		, r( r )
	{}

	Quaternion()
		: i( 0.0f )
		, j( 0.0f )
		, k( 0.0f )
		, r( 1.0f )
	{}

	Quaternion( const Vector3& vec, Float r )
	{
		SetAxisAngle( vec, r );
	}

	explicit Quaternion( Vector4 vec );

	Quaternion( Float xRotation, Float yRotation, Float zRotation );

	explicit Quaternion( const Vector3& eulerAngles );

	~Quaternion();

	Quaternion operator*( const Quaternion& q ) const;

	Vector4 operator*( const Vector4& v ) const
	{
		return Transform( v );
	}

	Vector3 operator*( const Vector3& v ) const
	{
		return Transform( v );
	}

	Quaternion operator*( Float val ) const;

	Quaternion operator/( Float val ) const;

	Quaternion operator+( const Quaternion& q ) const;

	Quaternion operator-( const Quaternion& q ) const;

	Bool operator==( const Quaternion& q ) const;
	Bool operator!=( const Quaternion& q ) const;

	Quaternion Inverted() const;
	void Invert();

	Float Normalize();
	Quaternion Normalized() const;

	Float Dot( const Quaternion& q ) const;

	Float Mag() const;
	Float SquareMag() const;

	Bool IsAlmostEqual( const Quaternion& q, Float eps = std::numeric_limits< Float >::epsilon() ) const;

	Vector3 ToEulerAngles() const;

	Vector3 GetXAxis3() const;
	Vector3 GetYAxis3() const;
	Vector3 GetZAxis3() const;

	Vector4 GetXAxis4() const;
	Vector4 GetYAxis4() const;
	Vector4 GetZAxis4() const;
};

