#pragma once

namespace forge::ai
{
	class NodeID
	{
	public:
		NodeID() = default;
		explicit NodeID( Uint32 value )
			: m_value( value )
		{}

		Bool IsValid() const
		{
			return c_invalidNodeID != m_value;
		}

		Uint32 Get() const
		{
			FORGE_ASSERT( IsValid() );
			return m_value;
		}

		auto operator<=>( const NodeID& other ) const = default;
		
	private:
		static constexpr Uint32 c_invalidNodeID = std::numeric_limits< Uint32 >::max();
		Uint32 m_value = c_invalidNodeID;
	};
}

namespace std
{
	template <>
	struct hash< forge::ai::NodeID >
	{
		size_t operator() ( const forge::ai::NodeID& nodeID ) const { return Math::CalculateHash( nodeID ); }
	};
}

namespace forge::ai
{
	struct Connection
	{
		NodeID m_to;
		Float m_cost;
	};

	using PathAsNodes = std::vector< NodeID >;
	using PathAsConnections = std::vector< Connection >;

	class Node
	{
	public:
		Node() {}
		~Node() {}

		const std::vector< Connection >& GetConnections() const { return m_connections; }

		const Connection* GetConnectionTo( const NodeID& destination ) const
		{
			auto it = std::find_if( m_connections.begin(), m_connections.end(), [ &destination ]( const Connection& element ) { return element.m_to == destination; } );

			if( it != m_connections.end() )
			{
				return &*it;
			}

			return nullptr;
		}

		void AddConnection( const Connection& connection )
		{
			auto it = std::find_if( m_connections.begin(), m_connections.end(),
				[ &connection ]( const Connection& element )
			{
				return element.m_to == connection.m_to;
			} );

			if( it == m_connections.end() )
			{
				m_connections.emplace_back( connection );
			}
			else if( connection.m_cost < it->m_cost )
			{
				*it = connection;
			}
		}

		Bool RemoveConnection( const NodeID& destination )
		{
			auto it = std::find_if( m_connections.begin(), m_connections.end(), [ &destination ]( const Connection& element ) { return element.m_to == destination; } );

			if( it != m_connections.end() )
			{
				m_connections.erase( it );
				return true;
			}

			return false;
		}

		void Enable( Bool enable )
		{
			m_enabled = enable;
		}

		Bool IsEnabled() const
		{
			return m_enabled;
		}

	private:
		std::vector< Connection > m_connections;
		Bool m_enabled = true;
	};

	template< class T >
	class NodeWithData : public Node
	{
	public:
		NodeWithData() {}

		NodeWithData( const T& data )
			: m_data ( data )
		{}

		~NodeWithData() {}

		const T& GetData() const
		{
			return m_data;
		}

	private:
		T m_data;
	};

	template< class NodeType >
	class Graph
	{
	public:
		Graph() {}
		virtual ~Graph() {}

		const std::vector< NodeType >& GetAllNodes() const
		{
			return m_nodes;
		}

		Uint32 GetNodesAmount() const
		{
			return static_cast< Uint32 >( m_nodes.size() );
		}

		const NodeType& GetNode( const NodeID& id ) const
		{
			return m_nodes[ id.Get() ];
		}

		void EnableNode( const NodeID& id, Bool enable )
		{
			GetNode( id ).Enable( enable );
		}

	protected:
		void AddConnection( const NodeID& from, const NodeID& to, Float cost )
		{
			m_nodes[ from.Get() ].AddConnection( { to, cost } );
		}

		NodeID AddEmptyNode()
		{
			return AddNode( NodeType() );
		}

	private:
		NodeID AddNode( const NodeType& node )
		{
			m_nodes.emplace_back( node );

			return NodeID( static_cast< Uint32 >( m_nodes.size() - 1 ) );
		}

		NodeType& GetNode( const NodeID& id )
		{
			return m_nodes[ id ];
		}

		std::vector< NodeType > m_nodes;
	};

	template< class T >
	class IdentifiedGraph : public Graph< Node >
	{

	public:
		IdentifiedGraph() {}
		~IdentifiedGraph() {}

		NodeID AddNode( const T& identifier )
		{
			NodeID id = AddEmptyNode();
			m_identifiersMap.emplace( identifier, id );
			m_nodesData.emplace_back( identifier );

			FORGE_ASSERT( m_nodesData.size() == GetAllNodes().size() );

			return id;
		}

		void AddConnection( const T& from, const T& to, Float cost )
		{
			NodeID fromId = GetOrCreateNode( from );
			NodeID toId = GetOrCreateNode( to );

			Graph< Node >::AddConnection( fromId, toId, cost );
		}

		const T& GetIdentifierFromID( NodeID id ) const
		{
			return m_nodesData[ id ];
		}

		std::vector< T > TranslatePath( const forge::ArraySpan< NodeID >& path )
		{
			std::vector< T > result;
			result.reserve( path.GetSize() );

			for( NodeID id : path )
			{
				result.emplace_back( GetIdentifierFromID( id ) );
			}

			return result;
		}

		Bool HasNodeForIdentifier( const T& identifier ) const
		{
			return m_identifiersMap.find( identifier ) != m_identifiersMap.end();
		}

		NodeID GetIDFromIdentifier( const T& identifier )
		{
			FORGE_ASSERT( HasNodeForIdentifier( identifier ) );

			return m_identifiersMap.find( identifier )->second;
		}

	private:
		NodeID GetOrCreateNode( const T& identifier )
		{
			auto it = m_identifiersMap.find( identifier );

			if( it != m_identifiersMap.end() )
			{
				return it->second;
			}

			return AddNode( identifier );;
		}

		std::unordered_map< T, NodeID > m_identifiersMap;
		std::vector< T > m_nodesData;
	};

	template< class T >
	class NavigationGraph : public Graph< Node >
	{
	public:
		NavigationGraph() {}
		~NavigationGraph() {}

		NodeID AddNode( const T& data )
		{
			NodeID id = AddEmptyNode();
			m_localizationData.emplace( data, id );
			m_nodesData.emplace_back( data );

			FORGE_ASSERT( m_nodesData.size() == GetAllNodes().size() );

			return id;
		}

		void AddConnection( const T& from, const T& to )
		{
			NodeID fromId = GetOrCreateNode( from );
			NodeID toId = GetOrCreateNode( to );

			Float cost = from.DistTo( to );

			Graph< Node>::AddConnection( fromId, toId, cost );
		}

		void AddConnection( const T& from, const T& to, Float cost )
		{
			NodeID fromId = GetOrCreateNode( from );
			NodeID toId = GetOrCreateNode( to );

			Graph< Node>::AddConnection( fromId, toId, cost );
		}

		std::vector< T > TranslatePath( forge::ArraySpan< const NodeID > path )
		{
			std::vector< T > result;
			result.reserve( path.GetSize() );

			for( NodeID id : path )
			{
				result.emplace_back( m_nodesData[ id.Get() ] );
			}

			return result;
		}

		Bool HasNodeForLocation( const T& location ) const
		{
			return m_localizationData.find( location ) != m_localizationData.end();
		}

		NodeID GetIDFromLocation( const T& location ) const
		{
			FORGE_ASSERT( HasNodeForLocation( location ) );

			return m_localizationData.find( location )->second;
		}

		const T& GetLocationFromID( NodeID id ) const
		{
			return m_nodesData[ id.Get() ];
		}

		NodeID GetOrCreateNode( const T& identifier )
		{
			auto it = m_localizationData.find( identifier );

			if( it != m_localizationData.end() )
			{
				return it->second;
			}

			return AddNode( identifier );
		}

	private:
		std::unordered_map< T, NodeID > m_localizationData;
		std::vector< T > m_nodesData;
	};
}