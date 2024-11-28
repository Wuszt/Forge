#pragma once
#include <vector>

namespace forge
{
	namespace allocator::internal
	{
		template< size_t Value, class T = void >
		struct MinUnsignedToStoreValue
		{
			using type = size_t;
		};

		template< size_t Value >
		struct MinUnsignedToStoreValue< Value, std::enable_if_t< Value < static_cast< Uint8 >( -1 ), void > >
		{
			static_assert( Value <= static_cast< Uint8 >( -1 ) );
			using type = Uint8;
		};

		template< size_t Value >
		struct MinUnsignedToStoreValue < Value, std::enable_if_t< static_cast< Uint8 >( -1 ) < Value - 1 && Value < static_cast< Uint16 >( -1 ), void > >
		{
			using type = Uint16;
		};

		template< size_t Value >
		struct MinUnsignedToStoreValue < Value, std::enable_if_t< static_cast< Uint16 >( -1 ) < Value - 1 && Value < static_cast< Uint32 >( -1 ), void > >
		{
			using type = Uint32;
		};
	}

#if 1
	template< class T, size_t StaticSize, template< class > class BaseAllocator = std::allocator >
	class InlineAllocator : public BaseAllocator< T >
	{
	public:
		using value_type = T;
		using TOffset = allocator::internal::MinUnsignedToStoreValue< StaticSize >::type;

		template< class U > 
		struct rebind 
		{
			using other = InlineAllocator< U, StaticSize, BaseAllocator >;
		};
		
		/// Is it really needed?
		InlineAllocator()
		{}

		template< class U >
		constexpr InlineAllocator( const InlineAllocator< U, StaticSize, BaseAllocator >& ) noexcept {}

		template< class U > bool operator==( const InlineAllocator< U, StaticSize, BaseAllocator >& ) const noexcept
		{
			return true;
		}
		template< class U > bool operator!=( const InlineAllocator< U, StaticSize, BaseAllocator>& ) const noexcept
		{
			return false;
		}
		///

		T* allocate( const size_t count )
		{
			if ( m_offsetInStaticBuffer != c_invalidOffset )
			{
				T* bufferPtr = reinterpret_cast< T* >( &m_staticBuffer[ 0 ] );
				const TOffset slack = StaticSize - m_offsetInStaticBuffer;
				if ( slack >= count )
				{
					bufferPtr = bufferPtr + m_offsetInStaticBuffer;
					m_offsetInStaticBuffer += static_cast< TOffset >( count );
					return reinterpret_cast< T* >( bufferPtr );
				}

				m_offsetInStaticBuffer = c_invalidOffset;
			}

			return BaseAllocator< T >::allocate( count );
		}

		void deallocate( T* const ptr, size_t count ) noexcept
		{
			if ( m_offsetInStaticBuffer == c_invalidOffset )
			{
				const T* staticBuffer = reinterpret_cast< const T* >( &m_staticBuffer[ 0 ] );

				if ( staticBuffer > ptr || ptr - staticBuffer > StaticSize )
				{
					BaseAllocator< T >::deallocate( ptr, count );
				}
			}
		}

	private:
		static constexpr TOffset c_invalidOffset = static_cast< TOffset >( -1 );
		alignas( T ) std::array< Byte, sizeof( T ) * StaticSize > m_staticBuffer;
		TOffset m_offsetInStaticBuffer = 0u;
	};
#else
	template< class T, size_t StaticSize, template< class > class BaseAllocator = std::allocator >
	class InlineAllocator : public BaseAllocator< T >
	{
	public:
		using value_type = T;
		using TOffset = size_t; //allocator::internal::MinUnsignedToStoreValue< StaticSize >::type;

		template< class U >
		struct rebind
		{
			using other = InlineAllocator< U, StaticSize, BaseAllocator >;
		};

		/// Is it really needed?
		InlineAllocator()
		{}

		template< class U >
		constexpr InlineAllocator( const InlineAllocator< U, StaticSize, BaseAllocator >& ) noexcept {}

		template< class U > bool operator==( const InlineAllocator< U, StaticSize, BaseAllocator >& ) const noexcept
		{
			return true;
		}
		template< class U > bool operator!=( const InlineAllocator< U, StaticSize, BaseAllocator>& ) const noexcept
		{
			return false;
		}
		///

		T* allocate( const size_t count )
		{
			if ( m_staticObjectsAmount != c_invalidOffset )
			{
				if ( m_staticObjectsAmount + count <= StaticSize )
				{
					m_staticObjectsAmount += static_cast< TOffset >( count );
					T* bufferPtr = reinterpret_cast< T* >( &m_staticBuffer[ 0 ] );

					bufferPtr = bufferPtr + ( StaticSize - m_staticObjectsAmount );
					return bufferPtr;
				}

				m_staticObjectsAmount = c_invalidOffset;
			}


			return BaseAllocator< T >::allocate( count );
		}

		void deallocate( T* const ptr, size_t count ) noexcept
		{
			if ( m_staticObjectsAmount == c_invalidOffset )
			{
				BaseAllocator< T >::deallocate( ptr, count );
			}
		}

	private:
		static constexpr TOffset c_invalidOffset = static_cast< TOffset >( -1 );
		alignas( T ) std::array< Byte, sizeof( T )* StaticSize > m_staticBuffer;
		TOffset m_staticObjectsAmount = 0;
	};
#endif
}