#include "Fpch.h"
#include "Random.h"
#include <random>
#include <ctime>

Math::Random::~Random() = default;

constexpr auto c_randRange = std::mt19937::max() - std::mt19937::min();

static_assert( std::numeric_limits< Uint32 >::max() <= std::mt19937::max(), "Max GetUnsigned is bigger than engine's max" );

Float Math::Random::GetFloat( Float min /*= 0.0f*/, Float max /*= 1.0f */ )
{
	auto random = (*m_randomEngine)();
	const Float range = max - min;
	return min + static_cast< Float >( static_cast< Double >( range ) * static_cast< Double >( random ) / static_cast< Double >( c_randRange ) );
}

Uint32 Math::Random::GetUnsigned( Uint32 min /*= 0u*/, Uint32 max /*= std::numeric_limits< Uint32 >::max() */ )
{
	auto random = ( *m_randomEngine )();
	const Uint32 range = max - min;
	return min + static_cast< Uint32 >( static_cast< Double >( range ) * static_cast< Double >( random ) / static_cast< Double >( c_randRange ) );
}

Math::Random::Random( Uint32 seed )
{
	m_randomEngine = std::make_unique<std::mt19937>( seed );
}

Math::Random::Random()
{
	m_randomEngine = std::make_unique<std::mt19937>( static_cast< Uint32 >( time( NULL ) ) );
}
