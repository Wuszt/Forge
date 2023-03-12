#pragma once
#include "Fragment.h"

namespace ecs
{
	class FragmentsPackage
	{
	public:
		FragmentsPackage( const rtti::IType& type, Uint32 initialSize = 0u )
			: m_fragments( forge::RawVector( type ) )
		{
			m_fragments.SetCapacity( 128u );
			m_fragments.Resize( initialSize );
		}

		void AddEmptyFragment()
		{
			m_fragments.AddEmpty();
		}

		void RemoveFragmentReorder( Uint32 index )
		{
			m_fragments.RemoveAtReorder( index );
		}

		const Fragment::Type& GetFragmentType() const
		{
			return static_cast< const Fragment::Type& >( m_fragments.GetType() );
		}

		template< class T >
		forge::ArraySpan< T > GetFragments()
		{
			return m_fragments.AsArraySpan< T >();
		}
		template< class T >
		forge::ArraySpan< const T > GetFragments() const
		{
			return m_fragments.AsArraySpan< const T >();
		}

		template< class T, class... Ts >
		void EmplaceFragment( Ts... data )
		{
			m_fragments.Emplace< T >( std::forward< Ts >( data )... );
		}

		void MoveFragment( Uint32 index, FragmentsPackage& source )
		{
			source.m_fragments.MoveElement( index, m_fragments.AddEmpty() );
		}

		Uint32 GetFragmentsCount() const
		{
			return m_fragments.GetSize();
		}

	private:
		forge::RawVector m_fragments;
	};
}