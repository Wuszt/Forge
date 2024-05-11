#pragma once

namespace ecs
{
	class Archetype;

	namespace fragmentViews::internal
	{
		void MarkArchetypeAsInUse( ecs::Archetype& archetype );
		void CancelArchetypeAsInUse( ecs::Archetype& archetype );
	}

	template< class T >
	class FragmentView
	{
	public:
		FragmentView( ecs::Archetype& archetype, const T* fragment )
		{
			if ( fragment )
			{
				m_archetype = &archetype;
				m_fragment = fragment;
				fragmentViews::internal::MarkArchetypeAsInUse( *m_archetype );
			}
		}

		FragmentView( const FragmentView& view )
			: m_archetype( view.m_archetype )
			, m_fragment( view.m_fragment )
		{
			if ( m_archetype )
			{
				fragmentViews::internal::MarkArchetypeAsInUse( *m_archetype );
			}
		}

		FragmentView( FragmentView&& view )
			: m_archetype( view.m_archetype )
			, m_fragment( view.m_fragment )
		{
			if ( m_archetype )
			{
				fragmentViews::internal::MarkArchetypeAsInUse( *m_archetype );
			}
		}

		~FragmentView()
		{
			if ( m_archetype )
			{
				fragmentViews::internal::CancelArchetypeAsInUse( *m_archetype );
			}
		}


		FragmentView& operator=( const FragmentView& view )
		{
			if ( m_archetype )
			{
				fragmentViews::internal::CancelArchetypeAsInUse( *m_archetype );
			}

			m_archetype = view.m_archetype;
			m_fragment = view.m_fragment;

			if ( m_archetype )
			{
				fragmentViews::internal::MarkArchetypeAsInUse( *m_archetype );
			}

			return *this;
		}

		const T* GetPtr() const
		{
			return m_fragment;
		}

		const T* operator->() const
		{
			return m_fragment;
		}

		const T& operator*() const
		{
			return *m_fragment;
		}

		operator bool() const
		{
			return m_fragment;
		}

	private:
		Archetype* m_archetype;
		const T* m_fragment = nullptr;
	};

	template< class T >
	class MutableFragmentView
	{
	public:
		MutableFragmentView( ecs::Archetype& archetype, T* fragment )
		{
			if ( fragment )
			{
				m_archetype = &archetype;
				m_fragment = fragment;
				fragmentViews::internal::MarkArchetypeAsInUse( *m_archetype );
			}
		}

		MutableFragmentView( const MutableFragmentView& view )
			: m_archetype( view.m_archetype )
			, m_fragment( view.m_fragment )
		{
			if( m_archetype )
			{
				fragmentViews::internal::MarkArchetypeAsInUse( *m_archetype );
			}
		}

		MutableFragmentView( MutableFragmentView&& view )
			: m_archetype( view.m_archetype )
			, m_fragment( view.m_fragment )
		{
			if ( m_archetype )
			{
				fragmentViews::internal::MarkArchetypeAsInUse( *m_archetype );
			}
		}

		~MutableFragmentView()
		{
			if ( m_archetype )
			{
				fragmentViews::internal::CancelArchetypeAsInUse( *m_archetype );
			}
		}

		MutableFragmentView& operator=( const MutableFragmentView& view )
		{
			if ( m_archetype )
			{
				fragmentViews::internal::CancelArchetypeAsInUse( *m_archetype );
			}

			m_archetype = view.m_archetype;
			m_fragment = view.m_fragment;
		
			if ( m_archetype )
			{
				fragmentViews::internal::MarkArchetypeAsInUse( *m_archetype );
			}

			return *this;
		}

		T* GetMutablePtr() const
		{
			return m_fragment;
		}

		T* operator->() const
		{
			return m_fragment;
		}

		T& operator*() const
		{
			return *m_fragment;
		}

		operator bool() const
		{
			return m_fragment;
		}

	private:
		Archetype* m_archetype;
		T* m_fragment = nullptr;
	};
}
