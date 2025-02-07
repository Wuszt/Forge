#pragma once

struct LinearColor
{
	RTTI_DECLARE_STRUCT( LinearColor );

	LinearColor() = default;
	LinearColor( Float r, Float g, Float b, Float a = 1.0f )
		: R( r )
		, G( g )
		, B( b )
		, A( a )
	{}

	Float R = 0.0f;
	Float G = 0.0f;
	Float B = 0.0f;
	Float A = 1.0f;

	LinearColor operator*( Float scalar ) const
	{
		return LinearColor( R * scalar, G * scalar, B * scalar, A * scalar );
	}

	static const LinearColor Red;
	static const LinearColor Green;
	static const LinearColor Blue;
};

inline const LinearColor LinearColor::Red = { 1.0f, 0.0f, 0.0f };
inline const LinearColor LinearColor::Green = { 0.0f, 1.0f, 0.0f };
inline const LinearColor LinearColor::Blue = { 0.0f, 0.0f, 1.0f };

