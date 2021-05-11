#pragma once
struct Quaternion
{   
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

    FORGE_INLINE static Quaternion IDENTITY() { return Quaternion( 0.0f, 0.0f, 0.0f, 1.0f ); }
    FORGE_INLINE static Quaternion I() { return Quaternion( 1.0f, 0.0f, 0.0f, 0.0f ); }
    FORGE_INLINE static Quaternion J() { return Quaternion( 0.0f, 1.0f, 0.0f, 0.0f ); }
    FORGE_INLINE static Quaternion K() { return Quaternion( 0.0f, 0.0f, 1.0f, 0.0f ); }

    Vector4 Transform( const Vector4& vec ) const;
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

    Quaternion( const Vector3 vec, Float r )
    {
		SetAxisAngle( vec, r );
	}

	Quaternion( const Vector4& vec );

	Quaternion( Float yaw, Float pitch, Float roll );

    ~Quaternion();

    Quaternion operator*( const Quaternion& q ) const;

	Quaternion Conjugate() const;

	Float Normalize();
};

