#pragma once

typedef Uint32 CallbackID;
constexpr CallbackID GetInvalidCallbackID()
{
	return std::numeric_limits< Uint32 >::max();
}

class ICallback
{
public:
	virtual ~ICallback() = default;
	virtual void RemoveListener( CallbackID id ) = 0;
};

class CallbackToken
{
public:
	CallbackToken( CallbackID id, ICallback& callback )
		: m_id( id )
		, m_callback( callback )
	{}

	CallbackToken( const CallbackToken& t ) = delete;
	CallbackToken( CallbackToken&& t )
		: m_id( t.m_id )
		, m_callback( t.m_callback )
	{
		t.m_id = GetInvalidCallbackID();
	}

	~CallbackToken()
	{
		if( m_id != GetInvalidCallbackID() )
		{
			Unregister();
		}
	}

	void Unregister()
	{
		m_callback.RemoveListener( m_id );
		m_id = GetInvalidCallbackID();
	}

private:
	CallbackID m_id = GetInvalidCallbackID();
	ICallback& m_callback;
};

template< class ...TParams >
class Callback : public ICallback
{
public:
	typedef std::function< void( TParams... ) > TFunc;

	Callback() = default;
	~Callback() = default;

	FORGE_INLINE void Invoke( TParams... params )
	{
		for( const auto& func : m_funcs )
		{
			func( std::forward< TParams >( params )... );
		}
	}

	FORGE_INLINE std::unique_ptr< CallbackToken > AddListener( const TFunc& func )
	{	
		m_funcs.emplace_back( func );
		
		if( m_nextFreeID == GetInvalidCallbackID() )
		{	
			m_idToFunc.emplace_back( m_funcs.size() - 1u );
			m_funcToID.emplace_back( m_idToFunc.back() );
			return std::make_unique< CallbackToken >( static_cast< CallbackID >( m_funcs.size() - 1u ), *this );
		}
		else
		{
			CallbackID nextFreeID = m_nextFreeID;
			m_nextFreeID = m_idToFunc[ m_nextFreeID ];
			m_idToFunc[ nextFreeID ] = m_funcs.size() - 1;
			m_funcToID.emplace_back( nextFreeID );
			return std::make_unique< CallbackToken >( nextFreeID, *this );
		}
	}

	FORGE_INLINE virtual void RemoveListener( CallbackID id ) override
	{
		std::swap( m_funcs[ m_idToFunc[ id ] ], m_funcs.back() );
		m_funcs.pop_back();

		std::swap( m_funcToID[ m_idToFunc[ id ] ], m_funcToID.back() );
		m_idToFunc[ m_funcToID[ m_idToFunc[ id ] ] ] = m_idToFunc[ id ];
		m_funcToID.pop_back();

		m_idToFunc[ id ] = m_nextFreeID;
		m_nextFreeID = id;
	}

	Uint32 GetListenersAmount() const
	{
		return m_funcs.size();
	}

private:
	const CallbackID c_invalidID = std::numeric_limits< Uint32 >::max();

	std::vector< Uint32 > m_idToFunc;
	std::vector< Uint32 > m_funcToID;

	std::vector< std::function< void( TParams... ) > > m_funcs;

	CallbackID m_nextFreeID = GetInvalidCallbackID();
};

