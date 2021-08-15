#include "fpch.h"
#include "FPSCounter.h"

namespace forge
{
	FPSCounter::FPSCounter()
	{
		std::fill( std::begin( m_buffer ), std::end( m_buffer ), -1.0f );
	}

	FPSCounter::~FPSCounter() = default;

	void FPSCounter::OnUpdateInternal( Float dt )
	{
		++m_counter;
		m_accumulator += dt;

		if( m_accumulator > 1.0f )
		{
			m_buffer[ ++m_bufferIndex % c_bufferSize ] = static_cast<Float>( m_counter ) / m_accumulator;
			m_counter = 0u;
			m_accumulator = 0.0f;
		}
	}
}
