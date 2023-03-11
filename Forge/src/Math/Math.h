#pragma once
#define FORGE_PI 3.141592653589793238462643383279502884197169f
#define FORGE_PI_HALF 1.57079632679f
#define FORGE_PI_TWO 6.28318530718f
#define DEG2RAD 0.01745329251f
#define RAD2DEG 57.295779513f

struct Vector2;
struct Vector3;
struct Vector4;
struct Quaternion;

namespace Math
{
	FORGE_INLINE Float Sin( Float rad )
	{
		return std::sin( rad );
	}

	FORGE_INLINE Float Cos( Float rad )
	{
		return std::cos( rad );
	}

	FORGE_INLINE Float Ctg( Float rad )
	{
		return 1.0f / std::tan( rad );
	}

	FORGE_INLINE Float Tg( Float rad )
	{
		return std::tan( rad );
	}

	FORGE_INLINE Float Acos( Float rad )
	{
		return std::acos( rad );
	}

	FORGE_INLINE Float Sign( Float value )
	{
		return std::copysign( 1.0f, value );
	}

	template< class T >
	FORGE_INLINE void Swap( T& l, T& r )
	{
		T tmp = l;
		l = r;
		r = tmp;
	}

	template< class T >
	FORGE_INLINE constexpr T Abs( T val )
	{
		return abs( val );
	}

	FORGE_INLINE constexpr Bool IsAlmostZero( Float val, Float epsilon = std::numeric_limits< Float >::epsilon() * 10.0f )
	{
		return Abs( val ) <= epsilon;
	}

	template< class T >
	FORGE_INLINE constexpr const T& Min( const T& l, const T& r )
	{
		return r > l ? l : r;
	}

	template< class T >
	FORGE_INLINE constexpr const T& Max( const T& l )
	{
		return l;
	}

	template< class T, class... Ts >
	FORGE_INLINE constexpr const T& Max( const T& l, const T& r, const Ts&... args )
	{
		return Max( r > l ? r : l, args... );
	}

	template< class T >
	FORGE_INLINE constexpr const T& Clamp( const T& min, const T& max, const T& value )
	{
		return Min( Max( min, value ), max );
	}

	template< class T0, class T1 >
	FORGE_INLINE constexpr T0 Pow( const T0& val, const T1& pow )
	{
		return std::pow( val, pow );
	}

	template< class T >
	FORGE_INLINE T Log10( const T& value )
	{
		return std::log10( value );
	}

	template< class T >
	FORGE_INLINE T Log2( const T& value )
	{
		return static_cast< T >( std::log2( value ) );
	}

	FORGE_INLINE Uint64 CombineHashes( Uint64 l, Uint64 r )
	{
		std::hash<Uint64> hasher;
		const Uint64 kMul = 0x9ddfea08eb382d69ULL;
		Uint64 a = ( static_cast< Uint64 >( hasher( r ) ) ^ l ) * kMul;
		a ^= ( a >> 47 );
		Uint64 b = ( l ^ a ) * kMul;
		b ^= ( b >> 47 );
		l = b * kMul;
		return l;
	}

	template< class T >
	FORGE_INLINE Uint64 CalculateHash( const T& value )
	{
		std::hash< T > hasher;
		return static_cast< Uint64 >( hasher( value ) );
	}

	template<>
	Uint64 CalculateHash( const Vector2& value );

	template<>
	Uint64 CalculateHash( const Vector3& value );

	template<>
	Uint64 CalculateHash( const Vector4& value );

	template< class T >
	FORGE_INLINE T Lerp( const T& from, const T& to, Float t )
	{
		t = Clamp( 0.0f, 1.0f, t );
		return from * ( 1.0f - t ) + to * t;
	}

	template<>
	Quaternion Lerp( const Quaternion& from, const Quaternion& to, Float t );

	FORGE_INLINE Float Truncate( Float value, Uint32 decimalPlaces )
	{
		decimalPlaces = Math::Min( static_cast< Uint32 >( std::numeric_limits< Double >::digits10 ), decimalPlaces );

		const Double multiplier = static_cast< double >( Math::Pow( 10.0, decimalPlaces ) );
		return static_cast< Float >( static_cast< Double >( static_cast< Int64 >( static_cast< Double >( value ) * multiplier ) ) / multiplier );
	}

	FORGE_INLINE Float Round( Float value, Uint32 decimalPlaces )
	{
		decimalPlaces = Math::Min( static_cast< Uint32 >( std::numeric_limits< Double >::digits10 ), decimalPlaces );

		const Double multiplier = Math::Pow( 10.0, decimalPlaces );
		return static_cast< Float >( std::round( static_cast< Double >( value ) * multiplier ) / multiplier );
	}
}
