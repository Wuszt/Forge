#pragma once

namespace rtti
{
	class IType;
}

namespace forge
{
	class IDataPackage
	{
	public:
		virtual ~IDataPackage() = default;
		virtual void AddEmptyData() = 0;
		virtual void RemoveDataReorder( Uint32 index ) = 0;
		virtual const rtti::IType& GetDataType() const = 0;
		virtual void MoveTo( Uint32 index, IDataPackage& destination ) = 0;
		virtual std::unique_ptr< IDataPackage > CreateNewInstance() const = 0;
	};

	template< class T >
	class DataPackage : public IDataPackage
	{
	public:
		DataPackage( Uint32 initialSize = 0u )
			: m_data( initialSize )
		{}

		T& operator[]( Uint32 index )
		{
			return m_data[ index ];
		}

		const T& operator[]( Uint32 index ) const
		{
			return m_data[ index ];
		}

		virtual void AddEmptyData() override
		{
			AddData();
		}

		virtual void RemoveDataReorder( Uint32 index ) override
		{
			forge::utils::RemoveReorder( m_data, index );
		}

		virtual const rtti::IType& GetDataType() const override
		{
			return T::GetTypeStatic();
		}

		virtual void MoveTo( Uint32 index, IDataPackage& destination ) override
		{
			FORGE_ASSERT( dynamic_cast<DataPackage< T >*>( &destination ) );
			DataPackage< T >* realType = static_cast<DataPackage< T >*>( &destination );
			realType->m_data.emplace_back( std::move( m_data[ index ] ) );
			RemoveDataReorder( index );
		}

		template< class... Ts >
		void AddData( Ts... data )
		{
			m_data.emplace_back( std::forward< Ts >( data )... );
		}

		typename std::vector< T >::iterator begin()
		{
			return m_data.begin();
		}

		typename std::vector< T >::iterator end()
		{
			return m_data.end();
		}

		typename std::vector< T >::const_iterator begin() const
		{
			return m_data.begin();
		}

		typename std::vector< T >::const_iterator end() const
		{
			return m_data.end();
		}

		Uint32 GetDataSize() const
		{
			return static_cast<Uint32>( m_data.size() );
		}

		std::unique_ptr< IDataPackage > CreateNewInstance() const override
		{
			return std::make_unique< DataPackage< T > >();
		}

	private:
		std::vector< T > m_data;
	};
}