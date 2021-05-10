#pragma once

namespace std
{
	template<class _Ty,
		size_t _Wx,
		size_t _Nx,
		size_t _Mx,
		size_t _Rx,
		_Ty _Px,
		size_t _Ux,
		_Ty _Dx,	// added
		size_t _Sx,
		_Ty _Bx,
		size_t _Tx,
		_Ty _Cx,
		size_t _Lx,
		_Ty _Fx>	// added
		class mersenne_twister_engine;

	typedef mersenne_twister_engine<unsigned int, 32, 624, 397, 31, 0x9908b0df,
		11, 0xffffffff, 7, 0x9d2c5680, 15, 0xefc60000, 18, 1812433253> mt19937;
}

namespace Math
{
	//todo: it might be not perfect...
	class Random
	{
	public:
		Random();
		Random( Uint32 seed );
		~Random();

		Float GetFloat( Float min = 0.0f, Float max = 1.0f );
		Uint32 GetUnsigned( Uint32 min = 0u, Uint32 max = std::numeric_limits< Uint32 >::max() );
        Uint32 GetRaw();

	private:
		std::unique_ptr< std::mt19937 > m_randomEngine;
	};

}