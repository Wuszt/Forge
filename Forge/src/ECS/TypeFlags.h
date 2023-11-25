#pragma once

namespace ecs
{
	template< class T, Uint32 Size >
	class TypeFlags
	{
		using MyType = TypeFlags< T, Size >;
	public:
		TypeFlags() = default;
		TypeFlags( std::initializer_list< const typename T::Type* > types )
		{
			for ( const T::Type* type : types )
			{
				Set( *type, true );
			}
		}

		static TypeFlags All()
		{
			return TypeFlags().Flipped();
		}

		void Set( const typename T::Type& type, bool value )
		{
			m_flags.set( T::GetTypeIndex( type ), value );
		}

		void Reset()
		{
			m_flags.reset();
		}

		Bool Test( Uint32 index ) const
		{
			return m_flags.test( index );
		}

		Bool Test( const typename T::Type& type ) const
		{
			return Test( T::GetTypeIndex( type ) );
		}

		const std::bitset< Size >& GetRawBits() const
		{
			return m_flags;
		}

		Bool Any() const
		{
			return m_flags.any();
		}

		void Flip()
		{
			m_flags.flip();
		}

		TypeFlags Flipped() const
		{
			TypeFlags result = *this;
			result.Flip();
			return result;
		}

		MyType operator&( const MyType& rTags ) const
		{
			MyType result;
			result.m_flags = m_flags & rTags.m_flags;
			return result;
		}

		MyType operator|( const MyType& rTags ) const
		{
			MyType result;
			result.m_flags = m_flags | rTags.m_flags;
			return result;
		}

		Bool operator==( const MyType& rTags ) const
		{
			return m_flags == rTags.m_flags;
		}

		Bool operator!=( const MyType& rTags ) const
		{
			return m_flags != rTags.m_flags;
		}

		Bool operator==( std::bitset< Size > rawBits ) const
		{
			return m_flags == rawBits;
		}

		Bool operator!=( std::bitset< Size > rawBits ) const
		{
			return m_flags != rawBits;
		}

		MyType& operator|=( const MyType& rTags )
		{
			m_flags |= rTags.m_flags;
			return *this;
		}

		MyType& operator&=( const MyType& rTags )
		{
			m_flags &= rTags.m_flags;
			return *this;
		}

		static constexpr Uint32 GetSize()
		{
			return Size;
		}

		void VisitSetTypes( std::function< void( Uint32 index ) > func ) const
		{
			Uint64 raw = m_flags.to_ullong();

			while ( raw != 0u )
			{
				Uint64 tmp = raw - 1u;
				Uint32 index = static_cast< Uint32 >( Math::Log2( raw & ~tmp ) );
				func( index );
				raw = raw & tmp;
			}
		}

		void VisitSetTypes( std::function< void( const typename T::Type& ) > func ) const
		{
			VisitSetTypes( [ & ]( Uint32 index )
				{
					func( *T::GetTypeWithIndex( index ) );
				} );
		}

	private:
		std::bitset< Size > m_flags;
	};
}

