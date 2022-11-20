#pragma once

namespace forge
{
	const Uint32 c_bufferSize = 30u;

	class FPSCounter
	{
	public:
		FPSCounter();
		~FPSCounter();

		void OnUpdateInternal( Float dt );

		static FPSCounter& GetFPSCounter()
		{
			static FPSCounter fpsCounter;
			return fpsCounter;
		}

		static void OnUpdate( Float dt )
		{
			GetFPSCounter().OnUpdateInternal( dt );
		}

		static Float GetAverageFPS( Uint32 averagingSeconds = 1u )
		{
			return GetFPSCounter().GetAverageFPSInternal( averagingSeconds );
		}

		Float GetAverageFPSInternal( Uint32 averagingSeconds = 1u ) const
		{
			FORGE_ASSERT( averagingSeconds <= c_bufferSize );
			FORGE_ASSERT( averagingSeconds >= 1 );

			Float acc = 0.0f;
			for( Uint32 i = 0; i < averagingSeconds; ++i )
			{
				Uint32 index = ( c_bufferSize + m_bufferIndex - i ) % c_bufferSize;
				acc += m_buffer[ index ];
			}

			return acc / static_cast<Float>( averagingSeconds );
		}

		Uint32 GetBufferSize() const
		{
			return c_bufferSize;
		}

		Float* GetBuffer( Uint32& outOffset, Uint32& outSize )
		{
			outOffset = ( m_bufferIndex + 1u ) % c_bufferSize;
			outSize = c_bufferSize;

			return m_buffer;
		}

	private:
		Uint32 m_counter = 0u;
		Float m_accumulator = 0.0f;

		Float m_buffer[ c_bufferSize ];
		Uint32 m_bufferIndex = 0u;
	};
}

