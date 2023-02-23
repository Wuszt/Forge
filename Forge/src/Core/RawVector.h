#pragma once

namespace forge
{
	class RawVector
	{
	public:
		template< class T >
		RawVector()
			: m_type( T::GetTypeStatic() )
		{}

		RawVector( const rtti::IType& type )
			: m_type( type )
		{}

		void Resize( Uint32 newSize )
		{
			Grow( newSize - GetSize() );

			for ( Uint32 i = GetSize(); i < newSize; ++i )
			{
				m_type.ConstructInPlace( m_data.GetData() + i * m_type.GetSize() );
			}

			for ( Uint32 i = newSize; i < GetSize(); ++i )
			{
				m_type.Destroy( m_data.GetData() + i * m_type.GetSize() );
			}

			m_size = newSize;
		}

		template< class T >
		T& Add( T value )
		{
			return Emplace< T >( std::move( value ) );
		}

		template< class T, class... TArgs >
		T& Emplace( TArgs... params )
		{
			FORGE_ASSERT( m_type.IsA< T >() );

			Grow( 1u );

			T* placeInBuffer = reinterpret_cast< T* >( m_data.GetData() ) + m_size;
			new ( placeInBuffer ) T( std::forward< TArgs >( params )... );

			++m_size;

			return *placeInBuffer;
		}

		void RemoveAt( Uint32 index )
		{
			FORGE_ASSERT( index < GetSize() );

			m_type.Destroy( m_data.GetData() + ( index * m_type.GetSize() ) );
			std::memmove( m_data.GetData() + ( index * m_type.GetSize() ), m_data.GetData() + ( index + 1u ) * m_type.GetSize(), ( GetSize() - index - 1u ) * m_type.GetSize() );

			--m_size;
		}

		void RemoveAtReorder( Uint32 index )
		{
			FORGE_ASSERT( index < GetSize() );

			m_type.Destroy( m_data.GetData() + ( index * m_type.GetSize() ) );

			if( index != GetSize() - 1u )
			{
				std::memmove( m_data.GetData() + ( index * m_type.GetSize() ), m_data.GetData() + ( GetSize() - 1u ) * m_type.GetSize(), m_type.GetSize() );
			}

			--m_size;
		}

		template< class T >
		forge::ArraySpan< T > AsArraySpan()
		{
			FORGE_ASSERT( m_type.IsA< T >() );
			return forge::ArraySpan< T >( reinterpret_cast< T* >( m_data.GetData() ), reinterpret_cast< T* >( m_data.GetData() + GetSize() * m_type.GetSize() ) );
		}

		template< class T >
		forge::ArraySpan< const T > AsArraySpan() const
		{
			FORGE_ASSERT( m_type.IsA< T >() );

			return forge::ArraySpan< const T >( static_cast< const T* >( m_data.GetData() ), static_cast< T* >( m_data.GetData() + GetSize() * m_type.GetSize() ) );
		}

		Uint32 GetSize() const
		{
			return m_size;
		}

		Uint32 GetCapacity() const
		{
			return static_cast< Uint32 >( m_data.GetSize() / static_cast< Uint64 >( m_type.GetSize() ) );
		}

	private:
		void SetCapacity( Uint32 newCapacity )
		{
			if ( newCapacity == GetCapacity() )
			{
				return;
			}

			Uint32 newMemorySize = newCapacity * m_type.GetSize();

			RawSmartPtr m_newData( newMemorySize );

			std::memcpy( m_newData.GetData(), m_data.GetData(), newMemorySize > m_data.GetSize() ? m_data.GetSize() : newMemorySize );
			m_data = std::move( m_newData );
		}

		void Grow( Uint32 sizeDelta )
		{
			Uint32 newSize = GetSize() + sizeDelta;
			while ( newSize > GetCapacity() )
			{
				constexpr Uint32 minCapacity = 4u;
				Uint32 twiceAsMuchCapacity = GetCapacity() * 2u;
				SetCapacity( minCapacity > twiceAsMuchCapacity ? minCapacity : twiceAsMuchCapacity );
			}
		}

		Uint32 m_size = 0u;
		const rtti::IType& m_type;
		RawSmartPtr m_data;
	};
}

