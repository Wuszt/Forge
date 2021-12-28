#pragma once
#define FORGE_PI 3.141592653589793238462643383279502884197169f
#define FORGE_PI_HALF 1.57079632679f
#define FORGE_PI_TWO 6.28318530718f
#define DEG2RAD 0.01745329251f

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

	FORGE_INLINE Float Ctg( Float rad )
	{
		return 1.0f / tan( rad );
	}

	FORGE_INLINE Float Tg( Float rad )
	{
		return tan( rad );
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

	template< class T >
	FORGE_INLINE const T& Min( const T& l, const T& r )
	{
		return r > l ? l : r;
	}

	template< class T >
	FORGE_INLINE const T& Max( const T& l )
	{
		return l;
	}

	template< class T, class... Ts >
	FORGE_INLINE const T& Max( const T& l, const T& r, const Ts&... args )
	{
		return Max( r > l ? r : l, args... );
	}

	template< class T >
	FORGE_INLINE const T& Clamp( const T& min, const T& max, const T& value )
	{
		return Min( Max( min, value ), max );
	}

	template< class T0, class T1 >
	FORGE_INLINE T0 Pow( const T0& val, const T1& pow )
	{
		return std::pow( val, pow );
	}

	FORGE_INLINE Uint32 CombineHashes( Uint32 l, Uint32 r )
	{
		return r ^( r + 0x9e3779b9 + ( l << 6 ) + ( l >> 2 ) );
	}

	template< class T >
	FORGE_INLINE Uint32 CalculateHash( const T& value )
	{
		std::hash< T > hasher;
		return static_cast< Uint32 >( hasher( value ) );
	}

	template<>
	FORGE_INLINE Uint32 CalculateHash( const Vector2& value )
	{
		return Math::CombineHashes( Math::CalculateHash( value.X ), Math::CalculateHash( value.Y ) );
	}

	template<>
	FORGE_INLINE Uint32 CalculateHash( const Vector3& value )
	{
		return Math::CombineHashes( Math::CombineHashes( Math::CalculateHash( value.X ), Math::CalculateHash( value.Y ) ), Math::CalculateHash( value.Z ) );
	}

	template<>
	FORGE_INLINE Uint32 CalculateHash( const Vector4& value )
	{
		return Math::CombineHashes( Math::CombineHashes( Math::CombineHashes( Math::CalculateHash( value.X ), Math::CalculateHash( value.Y ) ), Math::CalculateHash( value.Z ) ), Math::CalculateHash( value.W ) );
	}
}
