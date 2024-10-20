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
};

