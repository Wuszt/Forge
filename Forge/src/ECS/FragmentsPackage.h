#pragma once
#include "Fragment.h"

namespace ecs
{
	class IFragmentsPackage
	{
	public:
		virtual ~IFragmentsPackage() = default;
		virtual void AddEmptyData() = 0;
		virtual void RemoveDataReorder( Uint32 index ) = 0;
		virtual const Fragment::Type& GetDataType() const = 0;
		//virtual void MoveTo( Uint32 index, IFragmentsPackage& destination ) = 0;
		virtual std::unique_ptr< IFragmentsPackage > CreateNewInstance() const = 0;
	};

	template< class T >
	class FragmentsPackage : public IFragmentsPackage
	{
	public:
		T& operator[]( Uint32 index )
		{
			return m_fragments[ index ];
		}

		const T& operator[]( Uint32 index ) const
		{
			return m_fragments[ index ];
		}

		virtual void AddEmptyFragment() override
		{
			EmplaceFragment();
		}

		virtual void RemoveDataReorder( Uint32 index ) override
		{
			forge::utils::RemoveReorder( m_fragments, index );
		}

		virtual const Fragment::Type& GetFragmentType() const override
		{
			return T::GetTypeStatic();
		}

		template< class... Ts >
		void EmplaceFragment( Ts... data )
		{
			m_fragments.emplace_back( std::forward< Ts >( data )... );
		}

		/*
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

		virtual void MoveTo( Uint32 index, IDataPackage& destination ) override
		{
			FORGE_ASSERT( dynamic_cast< DataPackage< T >* >( &destination ) );
			DataPackage< T >* realType = static_cast< DataPackage< T >* >( &destination );
			realType->m_data.emplace_back( std::move( m_data[ index ] ) );
			RemoveDataReorder( index );
		}
		*/

		Uint32 GetFragmentsCount() const
		{
			return static_cast< Uint32 >( m_fragments.size() );
		}

		std::unique_ptr< IFragmentsPackage > Create() const override
		{
			return std::make_unique< DataPackage< T > >();
		}

	private:
		FragmentsPackage( Uint32 initialSize = 0u )
			: m_fragments( initialSize )
		{}

		std::vector< T > m_fragments;
	};
}