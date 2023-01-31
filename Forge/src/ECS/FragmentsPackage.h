#pragma once
#include "Fragment.h"

namespace ecs
{
	class IFragmentsPackage
	{
	public:
		IFragmentsPackage();
		virtual ~IFragmentsPackage() = 0;
		virtual void AddEmptyFragment() = 0;
		virtual void RemoveFragmentReorder( Uint32 index ) = 0;
		virtual const Fragment::Type& GetFragmentType() const = 0;
		virtual std::unique_ptr< IFragmentsPackage > GetEmptyCopy() const = 0;
		virtual void MoveFragment( Uint32 index, IFragmentsPackage& source ) = 0;
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

		forge::ArraySpan< T > GetFragments()
		{
			return m_fragments;
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

		virtual void MoveFragment( Uint32 index, IFragmentsPackage& source ) override
		{
			auto& typedSource = static_cast< FragmentsPackage< T >& >( source );
			m_fragments.emplace_back( std::move( typedSource.GetFragments()[ index ] ) );
		}

		Uint32 GetFragmentsCount() const
		{
			return static_cast< Uint32 >( m_fragments.size() );
		}

	private:
		std::vector< T > m_fragments;
	};
}