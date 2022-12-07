#pragma once
#include "Fragment.h"

namespace ecs
{
	class IFragmentsPackage
	{
	public:
		virtual ~IFragmentsPackage() = default;
		virtual void AddEmptyFragment() = 0;
		virtual void RemoveFragmentReorder( Uint32 index ) = 0;
		virtual const Fragment::Type& GetFragmentType() const = 0;
		virtual std::unique_ptr< IFragmentsPackage > GetEmptyCopy() const = 0;
		virtual void CopyFragment( Uint32 index, const IFragmentsPackage& source ) = 0;
	};

	template< class T >
	class FragmentsPackage : public IFragmentsPackage
	{
	public:
		FragmentsPackage( Uint32 initialSize = 0u )
			: m_fragments( initialSize )
		{}

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

		virtual void RemoveFragmentReorder( Uint32 index ) override
		{
			forge::utils::RemoveReorder( m_fragments, index );
		}

		virtual const Fragment::Type& GetFragmentType() const override
		{
			return T::GetTypeStatic();
		}

		forge::ArraySpan< const T > GetFragments() const
		{
			return m_fragments;
		}

		template< class... Ts >
		void EmplaceFragment( Ts... data )
		{
			m_fragments.emplace_back( std::forward< Ts >( data )... );
		}

		virtual std::unique_ptr< IFragmentsPackage > GetEmptyCopy() const override
		{
			return std::make_unique< FragmentsPackage< T > >( 0u );
		}

		virtual void CopyFragment( Uint32 index, const IFragmentsPackage& source ) override
		{
			const auto& typedSource = static_cast< const FragmentsPackage< T >& >( source );
			EmplaceFragment( typedSource.GetFragments()[ index ] );
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

	private:
		std::vector< T > m_fragments;
	};
}