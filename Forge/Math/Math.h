#pragma once
#define FORGE_PI 3.141592653589793238462643383279502884197169f
#define FORGE_PI_HALF 1.57079632679f
#define FORGE_PI_TWO 6.28318530718f

namespace Math
{
	FORGE_INLINE Float Sin( Float rad )
	{
		return sin( rad );
	}

	FORGE_INLINE Float Cos( Float rad )
	{
		return cos( rad );
	}

	template< class T >
	FORGE_INLINE void Swap( T& l, T& r )
	{
		T tmp = l;
		l = r;
		r = tmp;
	}

	template< class T >
	FORGE_INLINE T Abs( T val )
	{
		return abs( val );
	}

	FORGE_INLINE Bool IsAlmostZero( Float val, Float epsilon = std::numeric_limits< Float >::epsilon() * 10.0f )
	{
		return Abs( val ) <= epsilon;
	}
}