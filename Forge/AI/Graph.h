#pragma once

namespace AI
{
	class Graph
	{
	public:

		using NodeID = Uint32;

		struct Connection
		{
			NodeID m_to;
			Float m_cost;
		};

		class Node
		{
		public:
			Node();
			~Node();

			const std::vector< Connection >& GetAllConnections() const { return m_connections; }
			void AddConnection( const Connection& connection );
			Bool RemoveConnection( const NodeID& destination );

		private:
			std::vector< Connection > m_connections;
		};

		Graph();
		~Graph();

		void AddConnection( const NodeID& from, const NodeID& to, Float cost );
		NodeID AddEmptyNode();
		const std::vector< Node >& GetAllNodes() const { return m_nodes; }
		const Node& GetNode( const NodeID& id ) const;

	private:

		NodeID AddNode( const Node& node );
		Node& GetNode( const NodeID& id );

		std::vector< Node > m_nodes;
	};

	template< class T >
	class IdentifiedGraph
	{

	public:
		IdentifiedGraph() {}
		~IdentifiedGraph() {}

		Graph::NodeID AddNode( const T& identifier )
		{
			Graph::NodeID id = m_internalGraph.AddEmptyNode();
			m_identifiersMap.emplace( identifier, id );
			m_nodesData.emplace_back( identifier );

			assert( m_nodesData.size() == m_internalGraph.GetAllNodes().size() ); 

			return id;
		}

		void AddConnection( const T& from, const T& to, Float cost )
		{
			Graph::NodeID fromId = GetOrCreateNode( from );
			Graph::NodeID toId = GetOrCreateNode( to );

			m_internalGraph.AddConnection( fromId, toId, cost );
		}

		const Graph& GetInternalGraph() const
		{
			return m_internalGraph;
		}

		const T& GetIdentifierFromID( Graph::NodeID id ) const
		{
			return m_nodesData[ id ];
		}

		std::vector< T > TranslatePath( const std::vector< Graph::NodeID >& path )
		{
			std::vector< T > result;
			result.reserve( path.size() );

			for( Graph::NodeID id : path )
			{
				result.emplace_back( GetIdentifierFromID( id ) );
			}

			return result;
		}

		Graph::NodeID GetIDFromIdentifier( const T& identifier )
		{
			assert( m_identifiersMap.find( identifier ) != m_identifiersMap.end() );

			return m_identifiersMap.find( identifier )->second;
		}

	private:
		Graph::NodeID GetOrCreateNode( const T& identifier )
		{
			auto it = m_identifiersMap.find( identifier );

			if( it != m_identifiersMap.end() )
			{
				return it->second;
			}

			return AddNode( identifier );;
		}

		Graph m_internalGraph;
		std::unordered_map< T, Graph::NodeID > m_identifiersMap;
		std::vector< T > m_nodesData;
	};
}