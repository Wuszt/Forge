#pragma once

namespace forge
{
	template< class T >
	class ArraySpan
	{
	public:
		ArraySpan()
			: m_begin( nullptr )
			, m_end( nullptr )
		{}

		ArraySpan( const ArraySpan< T >& other )
			: m_begin( other.m_begin )
			, m_end( other.m_end )
		{}

		ArraySpan( T* begin, T* end )
			: m_begin( begin )
			, m_end( end )
		{}

		template< class TContainer >
		ArraySpan( TContainer& container )
			: ArraySpan( std::begin( container ), std::end( container ) )
		{}

		template< class TContainer >
		ArraySpan( const TContainer& container )
			: ArraySpan( std::begin( container ), std::end( container ) )
		{}

		template< class T >
		ArraySpan( std::initializer_list< T > container )
			: ArraySpan( std::begin( container ), std::end( container ) )
		{}

		template< class TIterator, typename = typename std::enable_if< !std::is_pointer< TIterator >::value >::type >
		ArraySpan( const TIterator& begin, const TIterator& end )
			: ArraySpan( begin._Unwrapped(), end._Unwrapped() )
		{}

		const T& operator[]( Uint32 index ) const { return *( m_begin + index ); }
		T& operator[]( Uint32 index ) { return *( m_begin + index ); }

		FORGE_INLINE Uint32 GetSize() const
		{
			return static_cast< Uint32 >( m_end - m_begin );
		}

		FORGE_INLINE T* begin()
		{
			return m_begin;
		}

		FORGE_INLINE T* end()
		{
			return m_end;
		}

		FORGE_INLINE const T* begin() const
		{
			return m_begin;
		}

		FORGE_INLINE const T* end() const
		{
			return m_end;
		}

	private:
		T* m_begin;
		T* m_end;
	};
}

