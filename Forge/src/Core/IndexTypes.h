#pragma once

namespace forge
{
	namespace internal
	{
		template< class T >
		class IndexBase
		{
			static_assert( std::is_integral< T >::value, "Index type must be integral" );
			static_assert( std::is_unsigned< T >::value, "Index type must be unsigned" );
			static_assert( std::is_arithmetic< T >::value, "Index type must be arithmetic" );

		public:
			IndexBase()
			{}

			IndexBase( T index )
				: m_value( index )
			{}

			Bool IsValid() const
			{
				return m_value != c_invalid;
			}

			T Get() const
			{
				return m_value;
			}

			Uint32 AsUint32() const
			{
				return static_cast< Uint32 >( m_value );
			}

		private:
			static constexpr T c_invalid = std::numeric_limits< T >::max();
			T m_value = c_invalid;
		};

	}

		using Index8 = internal::IndexBase< Uint8 >;
		using Index16 = internal::IndexBase< Uint16 >;
		using Index32 = internal::IndexBase< Uint32 >;
		using Index64 = internal::IndexBase< Uint64 >;
}