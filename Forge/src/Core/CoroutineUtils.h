#pragma once
#include <coroutine>

namespace forge::co
{
	template< class T >
	class Generator
	{
	public:
		struct promise_type
		{
			Generator get_return_object() { return Generator( *this ); }
			std::suspend_always initial_suspend() noexcept { return {}; }
			std::suspend_always final_suspend() noexcept { return {}; }
			void return_void() {}
			void unhandled_exception() {}

			std::suspend_always yield_value( T val )
			{
				m_value = std::move( val );
				return {};
			}

			T m_value;
		};

		Generator( promise_type& promise )
			: m_handle( std::coroutine_handle< promise_type >::from_promise( promise ) )
		{}

		Generator( Generator&& gen )
			: m_handle( std::move( gen.m_handle ) )
		{
			gen.m_handle = {};
		}

		Generator& operator=( Generator&& gen )
		{
			~Generator();
			m_handle = std::move( gen.m_handle );
			gen.m_handle = {};
			return *this;
		}

		~Generator()
		{
			if ( m_handle )
			{
				m_handle.destroy();
			}
		}

		struct Iterator
		{
			Iterator( std::coroutine_handle< promise_type > handle )
				: m_handle( handle )
			{
			}

			Iterator( Iterator&& it )
				: m_handle( std::move( it.m_handle ) )
			{
				it.m_handle = {};
			}

			Iterator& operator=( Iterator&& it )
			{
				~Iterator();
				m_handle = std::move( it.m_handle );
				it.m_handle = {};
			}

			~Iterator()
			{
				if ( m_handle )
				{
					m_handle.destroy();
				}
			}

			T& operator*()
			{
				return m_handle.promise().m_value;
			}

			bool operator==( std::default_sentinel_t ) const
			{
				return !m_handle || m_handle.done();
			}

			Iterator& operator++()
			{
				Resume();

				return *this;
			}

			void Resume()
			{
				PC_SCOPE_FUNC();

				if ( m_handle )
				{
					m_handle.resume();
				}
			}

			std::coroutine_handle< promise_type > m_handle;
		};

		Iterator begin()
		{
			if ( !m_handle || m_handle.done() )
			{
				Iterator( {} );
			}

			Iterator it( std::move( m_handle ) );
			it.Resume();
			m_handle = {};
			return it;
		}

		std::default_sentinel_t end() { return {}; }

	private:
		std::coroutine_handle< promise_type > m_handle;
	};
}