#pragma once
#define FORGE_PI 3.141592653589793238462643383279502884197169f
#define FORGE_PI_HALF 1.57079632679f

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
}
