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
				m_type.ConstructInPlace( GetRawDataUnsafe( i ) );
			}

			for ( Uint32 i = newSize; i < GetSize(); ++i )
			{
				m_type.Destroy( GetRawDataUnsafe( i ) );
			}

			m_size = newSize;
		}

		template< class T >
		T& Add( T value )
		{
			return Emplace< T >( std::move( value ) );
		}

		void* AddEmpty()
		{
			Grow( 1u );

			void* placeInBuffer = GetRawDataUnsafe( GetSize() );
			m_type.ConstructInPlace( placeInBuffer );

			++m_size;

			return GetRawData( GetSize() - 1u );
		}

		template< class T, class... TArgs >
		T& Emplace( TArgs... params )
		{
			FORGE_ASSERT( m_type.IsA< T >() );

			Grow( 1u );

			T* placeInBuffer = static_cast< T* >( GetRawDataUnsafe( GetSize() ) );
			new ( placeInBuffer ) T( std::forward< TArgs >( params )... );

			++m_size;

			return *placeInBuffer;
		}

		void RemoveAt( Uint32 index )
		{
			FORGE_ASSERT( index < GetSize() );

			m_type.Destroy( GetRawData( index ) );
			std::memmove( GetRawData( index ), GetRawDataUnsafe( index + 1u ), ( GetSize() - index - 1u ) * m_type.GetSize() );

			--m_size;
		}

		void RemoveAtReorder( Uint32 index )
		{
			FORGE_ASSERT( index < GetSize() );

			m_type.Destroy( GetRawData( index ) );

			if( index != GetSize() - 1u )
			{
				std::memmove( GetRawData( index ), GetRawData( GetSize() - 1u ), m_type.GetSize() );
			}

			--m_size;
		}

		template< class T >
		forge::ArraySpan< T > AsArraySpan()
		{
			FORGE_ASSERT( m_type.IsA< T >() );
			return forge::ArraySpan< T >( static_cast< T* >( m_data.GetData() ), static_cast< T* >( GetRawDataUnsafe( GetSize() ) ) );
		}

		template< class T >
		forge::ArraySpan< const T > AsArraySpan() const
		{
			FORGE_ASSERT( m_type.IsA< T >() );

			return forge::ArraySpan< const T >( static_cast< const T* >( m_data.GetData() ), static_cast< const T* >( GetRawDataUnsafe( GetSize() ) ) );
		}

		void* GetRawData( Uint32 index ) const
		{
			FORGE_ASSERT( index < GetSize() );
			return GetRawDataUnsafe( index );
		}

		Uint32 GetSize() const
		{
			return m_size;
		}

		Uint32 GetCapacity() const
		{
			return static_cast< Uint32 >( m_data.GetSize() / static_cast< Uint64 >( m_type.GetSize() ) );
		}

		const rtti::IType& GetType() const
		{
			return m_type;
		}

		void MoveElement( Uint32 index, void* destination )
		{
			m_type.MoveInPlace( destination, GetRawData( index ) );
		}

		void SetCapacity( Uint32 newCapacity )
		{
			if ( newCapacity == GetCapacity() )
			{
				return;
			}

			Uint32 newMemorySize = newCapacity * m_type.GetSize();

			RawSmartPtr m_newData( newMemorySize );

			if ( GetSize() > newCapacity )
			{
				Resize( newCapacity );
			}

			for ( Uint32 i = 0u; i < GetSize(); ++i )
			{
				Uint32 offset = m_type.GetSize() * i;
				m_type.MoveInPlace( static_cast< Byte* >( m_newData.GetData() ) + offset, static_cast< Byte* >( m_data.GetData() ) + offset );
			}

			m_data = std::move( m_newData );
		}

		void Reserve( Uint32 amount )
		{
			if ( amount > GetCapacity() )
			{
				SetCapacity( amount );
			}
		}

	private:
		void* GetRawDataUnsafe( Uint32 index ) const
		{
			return static_cast< Byte* >( m_data.GetData() ) + ( index * m_type.GetSize() );
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

