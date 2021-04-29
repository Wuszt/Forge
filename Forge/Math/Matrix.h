#pragma once
#include "Fpch.h"

#include "../Math/Vector4.h"

class MATH_API Matrix
{
public:

	static Matrix IDENTITY()
	{
		return Matrix( Vector4::EX(), Vector4::EY(), Vector4::EZ(), Vector4::EW() );
	}

	Matrix() {}

	Matrix( const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w )
		: X( x )
		, Y( y )
		, Z( z )
		, W( w )
	{}

	~Matrix() {}

	void SetIdentity()
	{
		*this = IDENTITY();
	}

	Vector4 X = Vector4::EX();
	Vector4 Y = Vector4::EY();
	Vector4 Z = Vector4::EZ();
	Vector4 W = Vector4::EW();

	Vector4& operator[]( Uint32 index ) const
	{
		Vector4* ptr = const_cast< Vector4* >( &X );
		return *( ptr + index );
	}
};

