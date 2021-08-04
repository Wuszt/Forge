#pragma once

template< class ...TParams >
class Callback
{
public:
	typedef Uint32 ID;
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

	FORGE_INLINE ID AddListener( const TFunc& func )
	{	
		m_funcs.emplace_back( func );
		
		if( m_nextFreeID == c_invalidID )
		{	
			m_idToFunc.emplace_back( m_funcs.size() - 1u );
			m_funcToID.emplace_back( m_idToFunc.back() );
			return m_funcs.size() - 1u;
		}
		else
		{
			ID nextFreeID = m_nextFreeID;
			m_nextFreeID = m_idToFunc[ m_nextFreeID ];
			m_idToFunc[ nextFreeID ] = m_funcs.size() - 1;
			m_funcToID.emplace_back( nextFreeID );
			return nextFreeID;
		}
	}

	FORGE_INLINE void RemoveListener( ID id )
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
	const ID c_invalidID = std::numeric_limits< Uint32 >::max();

	std::vector< Uint32 > m_idToFunc;
	std::vector< Uint32 > m_funcToID;

	std::vector< std::function< void( TParams... ) > > m_funcs;

	ID m_nextFreeID = c_invalidID;
};

