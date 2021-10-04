#pragma once

namespace forge
{
	class IDataPackage
	{
	public:
		virtual ~IDataPackage() = default;
		virtual void AddEmptyData() = 0;
		virtual void RemoveDataReorder( Uint32 index ) = 0;
		virtual std::type_index GetTypeIndex() const = 0;
		virtual void MoveTo( Uint32 index, IDataPackage& destination ) = 0;
	};

	template< class T >
	class DataPackage : public IDataPackage
	{
	public:
		DataPackage( Uint32 initialSize = 0u )
			: m_data( initialSize )
		{}

		FORGE_INLINE T& operator[]( Uint32 index )
		{
			return m_data[ index ];
		}

		FORGE_INLINE const T& operator[]( Uint32 index ) const
		{
			return m_data[ index ];
		}

		FORGE_INLINE virtual void AddEmptyData() override
		{
			AddData();
		}

		FORGE_INLINE virtual void RemoveDataReorder( Uint32 index ) override
		{
			std::swap( m_data[ index ], m_data.back() );
			m_data.pop_back();
		}

		FORGE_INLINE virtual std::type_index GetTypeIndex() const override
		{
			return typeid( T );
		}

		FORGE_INLINE virtual void MoveTo( Uint32 index, IDataPackage& destination ) override
		{
			FORGE_ASSERT( dynamic_cast<DataPackage< T >*>( &destination ) );
			DataPackage< T >* realType = static_cast<DataPackage< T >*>( &destination );
			realType->m_data.emplace_back( std::move( m_data[ index ] ) );
			RemoveDataReorder( index );
		}

		template< class... Ts >
		FORGE_INLINE void AddData( Ts... data )
		{
			m_data.emplace_back( std::forward< Ts >( data )... );
		}

		FORGE_INLINE typename std::vector< T >::iterator begin()
		{
			return m_data.begin();
		}

		FORGE_INLINE typename std::vector< T >::iterator end()
		{
			return m_data.end();
		}

		FORGE_INLINE Uint32 GetDataSize() const
		{
			return static_cast<Uint32>( m_data.size() );
		}

	private:
		std::vector< T > m_data;
	};
}