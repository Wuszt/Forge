#pragma once

namespace forge
{
	typedef Uint32 CallbackID;
	constexpr CallbackID GetInvalidCallbackID()
	{
		return std::numeric_limits< Uint32 >::max();
	}

	class CallbackToken
	{
	public:
		class ICallbackImpl
		{
		public:
			virtual ~ICallbackImpl() = default;
			virtual void RemoveListener( CallbackID id ) = 0;
		};

		CallbackToken( CallbackID id, std::weak_ptr< ICallbackImpl > callback )
			: m_id( id )
			, m_callback( callback )
		{}

		CallbackToken() = default;
		CallbackToken( const CallbackToken& t ) = delete;
		CallbackToken( CallbackToken&& t )
			: m_id( t.m_id )
			, m_callback( t.m_callback )
		{
			t.m_id = GetInvalidCallbackID();
			t.m_callback.reset();
		}

		CallbackToken& operator=( CallbackToken&& t )
		{
			swap( *this, t );

			return *this;
		}

		~CallbackToken()
		{
			Unregister();
		}

		void Unregister()
		{
			if( std::shared_ptr< ICallbackImpl > callback = m_callback.lock() )
			{
				callback->RemoveListener( m_id );
				m_id = GetInvalidCallbackID();
				m_callback.reset();
			}
		}

		Bool IsValid()
		{
			return !m_callback.expired();
		}

	private:
		CallbackID m_id = GetInvalidCallbackID();
		std::weak_ptr< ICallbackImpl > m_callback;
		
		friend void swap( CallbackToken& l, CallbackToken& r )
		{
			std::swap( l.m_callback, r.m_callback );
			std::swap( l.m_id, r.m_id );
		}
	};

	template< class ...TParams >
	class Callback
	{
	public:
		using TFunc = std::function< void( TParams... ) >;

		Callback() = default;
		Callback( const Callback& callback ) = delete;
		Callback( Callback&& callback ) = default;
		~Callback() = default;

		Callback& operator=( const Callback& callback ) = delete;
		Callback& operator=( Callback&& callback ) = default;

		void Invoke( TParams... params ) const
		{
			m_implementation->Invoke( params... );
		}

		[[nodiscard]] CallbackToken AddListener( TFunc func )
		{
			return CallbackToken( m_implementation->AddListener( std::move( func ) ), m_implementation );
		}

		void RemoveListener( CallbackID id )
		{
			m_implementation->RemoveListener( id );
		}

		Uint32 GetListenersAmount() const
		{
			return m_implementation->GetListenersAmount();
		}

	private:
		class CallbackImpl : public CallbackToken::ICallbackImpl
		{
		public:
			void Invoke( TParams... params ) const
			{
				for( const auto& func : m_funcs )
				{
					func( std::forward< TParams >( params )... );
				}
			}

			CallbackID AddListener( TFunc func )
			{
				m_funcs.emplace_back( std::move( func ) );

				if( m_nextFreeID == GetInvalidCallbackID() )
				{
					m_idToFunc.emplace_back( static_cast<Uint32>( m_funcs.size() - 1u ) );
					m_funcToID.emplace_back( m_idToFunc.back() );
					return  static_cast<CallbackID>( static_cast<Uint32>( m_funcs.size() ) - 1u );
				}
				else
				{
					CallbackID nextFreeID = m_nextFreeID;
					m_nextFreeID = m_idToFunc[ m_nextFreeID ];
					m_idToFunc[ nextFreeID ] = static_cast<Uint32>( m_funcs.size() ) - 1;
					m_funcToID.emplace_back( nextFreeID );
					return nextFreeID;
				}
			}

			virtual void RemoveListener( CallbackID id ) override
			{
				forge::utils::RemoveReorder( m_funcs, m_idToFunc[ id ] );

				m_idToFunc[ m_funcToID.back() ] = m_idToFunc[ id ];

				forge::utils::RemoveReorder( m_funcToID, m_idToFunc[ id ] );

				m_idToFunc[ id ] = m_nextFreeID;
				m_nextFreeID = id;
			}

			Uint32 GetListenersAmount() const
			{
				return static_cast<Uint32>( m_funcs.size() );
			}

		private:
			const CallbackID c_invalidID = std::numeric_limits< Uint32 >::max();

			std::vector< Uint32 > m_idToFunc;
			std::vector< Uint32 > m_funcToID;

			std::vector< TFunc > m_funcs;

			CallbackID m_nextFreeID = GetInvalidCallbackID();
		};

		std::shared_ptr< CallbackImpl > m_implementation = std::make_shared< CallbackImpl >();
	};
}

