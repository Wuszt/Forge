#pragma once

namespace AI
{
	using NodeID = Uint32;

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

		const std::vector< Connection >& GetAllConnections() const { return m_connections; }

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

	private:
		std::vector< Connection > m_connections;
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
		~Graph() {}

		void AddConnection( const NodeID& from, const NodeID& to, Float cost )
		{
			m_nodes[ from ].AddConnection( { to, cost } );
		}

		NodeID AddEmptyNode()
		{
			return AddNode( NodeType() );
		}

		const std::vector< NodeType >& GetAllNodes() const
		{
			return m_nodes;
		}

		const NodeType& GetNode( const NodeID& id ) const
		{
			return m_nodes[ id ];
		}

	private:
		NodeID AddNode( const NodeType& node )
		{
			m_nodes.emplace_back( node );

			return static_cast<Uint32>( m_nodes.size() - 1 );
		}

		NodeType& GetNode( const NodeID& id )
		{
			return m_nodes[ id ];
		}

		std::vector< NodeType > m_nodes;
	};

	template< class T, class NodeType = Node >
	class IdentifiedGraph
	{

	public:
		IdentifiedGraph() {}
		~IdentifiedGraph() {}

		NodeID AddNode( const T& identifier )
		{
			NodeID id = m_internalGraph.AddEmptyNode();
			m_identifiersMap.emplace( identifier, id );
			m_nodesData.emplace_back( identifier );

			FORGE_ASSERT( m_nodesData.size() == m_internalGraph.GetAllNodes().size() );

			return id;
		}

		void AddConnection( const T& from, const T& to, Float cost )
		{
			NodeID fromId = GetOrCreateNode( from );
			NodeID toId = GetOrCreateNode( to );

			m_internalGraph.AddConnection( fromId, toId, cost );
		}

		const Graph< NodeType >& GetInternalGraph() const
		{
			return m_internalGraph;
		}

		const T& GetIdentifierFromID( NodeID id ) const
		{
			return m_nodesData[ id ];
		}

		std::vector< T > TranslatePath( const std::vector< NodeID >& path )
		{
			std::vector< T > result;
			result.reserve( path.size() );

			for( NodeID id : path )
			{
				result.emplace_back( GetIdentifierFromID( id ) );
			}

			return result;
		}

		NodeID GetIDFromIdentifier( const T& identifier )
		{
			FORGE_ASSERT( m_identifiersMap.find( identifier ) != m_identifiersMap.end() );

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

		Graph< NodeType > m_internalGraph;
		std::unordered_map< T, NodeID > m_identifiersMap;
		std::vector< T > m_nodesData;
	};
}