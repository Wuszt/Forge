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

		ArraySpan( T* begin, Uint32 length )
			: m_begin( begin )
			, m_end( begin + length )
		{}

		template< class TContainer >
		ArraySpan( TContainer& container )
			: ArraySpan( std::begin( container ), std::end( container ) )
		{}

		template< class TContainer >
		ArraySpan( const TContainer& container )
			: ArraySpan( std::begin( container ), std::end( container ) )
		{}

		template< class TContainer >
		ArraySpan( TContainer& container, Uint32 length )
			: ArraySpan( std::begin( container ), std::begin( container ) + length )
		{}

		template< class TContainer >
		ArraySpan( const TContainer& container, Uint32 length )
			: ArraySpan( std::begin( container ), std::begin( container ) + length )
		{}

		template< class T >
		ArraySpan( std::initializer_list< T > container )
			: ArraySpan( const_cast< T* >( std::begin( container ) ),
				const_cast< T* >( std::end( container ) ) )
		{}

		template< class TIterator, typename = typename std::enable_if< !std::is_pointer< TIterator >::value >::type >
		ArraySpan( const TIterator& begin, const TIterator& end )
			: ArraySpan( begin._Unwrapped(), end._Unwrapped() )
		{}

		const T& operator[]( Uint32 index ) const { return *( m_begin + index ); }
		T& operator[]( Uint32 index ) { return *( m_begin + index ); }

		Uint32 GetSize() const
		{
			return static_cast< Uint32 >( m_end - m_begin );
		}

		Bool IsEmpty() const
		{
			return m_begin == m_end;
		}

		T* begin()
		{
			return m_begin;
		}

		T* end()
		{
			return m_end;
		}

		const T* begin() const
		{
			return m_begin;
		}

		const T* end() const
		{
			return m_end;
		}

		T* back()
		{
			return m_begin + GetSize() - 1u;
		}

		const T* back() const
		{
			return m_begin + GetSize() - 1u;
		}

	private:
		T* m_begin;
		T* m_end;
	};
}

