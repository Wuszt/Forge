#pragma once
#include "Graph.h"

namespace AI
{
	namespace
	{
		struct DijkstraNodeData
		{
			Float m_cost;
			NodeID m_predecessor;
		};

		struct AStarNodeData
		{
			Float m_localHeuristicCost = -1.0f;
			Float m_heuristicCost;
			Float m_cost;
			NodeID m_predecessor;
		};
	}

	template< class DataType >
	static void GetPaths( const forge::ArraySpan< const DataType >& nodesData, std::vector< PathAsNodes >& outPaths )
	{
		for ( Uint32 i = 0; i < nodesData.GetSize(); ++i )
		{
			PathAsNodes path;

			if ( nodesData[ i ].m_cost != std::numeric_limits< Float >::infinity() )
			{
				auto currentNode = i;

				while ( nodesData[ currentNode ].m_cost > 0.0f )
				{
					path.emplace_back( currentNode );
					currentNode = nodesData[ currentNode ].m_predecessor;
				}

				path.emplace_back( currentNode );
				std::reverse( path.begin(), path.end() );
			}

			outPaths.emplace_back( std::move( path ) );
		}
	}

	template< class GraphType, class DataType >
	static void GetPaths( const GraphType& graph, const forge::ArraySpan< const DataType >& nodesData, std::vector< PathAsConnections >& outPaths )
	{
		for ( Uint32 i = 0; i < nodesData.GetSize(); ++i )
		{
			PathAsConnections path;

			if ( nodesData[ i ].m_cost != std::numeric_limits< Float >::infinity() )
			{
				auto currentNode = i;

				while ( nodesData[ currentNode ].m_cost > 0.0f )
				{
					const auto& node = graph.GetNode( nodesData[ currentNode ].m_predecessor );
					path.emplace_back( *node.GetConnectionTo( currentNode ) );
					currentNode = nodesData[ currentNode ].m_predecessor;
				}

				std::reverse( path.begin(), path.end() );
			}

			outPaths.emplace_back( std::move( path ) );
		}
	}

	template< class GraphType >
	static std::vector< DijkstraNodeData > PerformDijkstraInternal( NodeID startNode, const GraphType& graph )
	{
		Uint32 nodesAmount = graph.GetNodesAmount();

		std::vector< DijkstraNodeData > nodesData;
		nodesData.resize( nodesAmount, { std::numeric_limits< Float >::infinity(), std::numeric_limits< NodeID >::max() } );

		nodesData[ startNode ].m_predecessor = startNode;
		nodesData[ startNode ].m_cost = 0.0f;

		std::vector< NodeID > queue;
		queue.reserve( nodesAmount );

		queue.emplace_back( startNode );

		std::vector< Bool > scoredNodes;
		scoredNodes.resize( nodesAmount, false );

		struct Pred
		{
			const std::vector< DijkstraNodeData >& m_nodesData;

			bool operator () ( Float value, NodeID id ) const {
				return m_nodesData[ id ].m_cost < value;
			}

			bool operator () ( NodeID id, Float value ) const {
				return m_nodesData[ id ].m_cost > value;
			}
		};

		while ( !queue.empty() )
		{
			auto minIt = queue.back();
			queue.pop_back();

			NodeID currentNode = minIt;
			const DijkstraNodeData& currentNodeData = nodesData[ currentNode ];

			for ( const auto& connection : graph.GetNode( currentNode ).GetAllConnections() )
			{

				DijkstraNodeData& destinationData = nodesData[ connection.m_to ];

				Float overallCost = currentNodeData.m_cost + connection.m_cost;
				if ( destinationData.m_cost > overallCost )
				{
					Float previousScore = destinationData.m_cost;

					if ( scoredNodes[ connection.m_to ] )
					{
						auto pairIt = std::equal_range( queue.begin(), queue.end(), destinationData.m_cost, Pred{ nodesData } );

						auto it = std::find( pairIt.first, pairIt.second, connection.m_to );

						queue.erase( it );
					}

					destinationData.m_cost = overallCost;
					destinationData.m_predecessor = currentNode;

					auto it = std::upper_bound( queue.begin(), queue.end(), overallCost, Pred{ nodesData } );
					queue.emplace( it, connection.m_to );

					scoredNodes[ connection.m_to ] = true;
				}
			}
		}

		return nodesData;
	}

	template< class GraphType >
	void PerformDijkstra( NodeID startNode, const GraphType& graph, std::vector< PathAsNodes >& outPaths )
	{
		std::vector< DijkstraNodeData > data = PerformDijkstraInternal( startNode, graph );
		GetPaths( forge::ArraySpan< const DijkstraNodeData >( data ), outPaths );
	}

	template< class GraphType >
	void PerformDijkstra( NodeID startNode, const GraphType& graph, std::vector< PathAsConnections >& outPaths )
	{
		std::vector< DijkstraNodeData > data = PerformDijkstraInternal( startNode, graph );
		GetPaths( graph, forge::ArraySpan< const DijkstraNodeData >( data ), outPaths );
	}

	template< class DataType >
	Float BasicHeuristicFormula( const DataType& from, const DataType& to )
	{
		return from.DistanceSquaredTo( to );
	}

	FORGE_INLINE Float ManhattanHeuristicFormula( const Vector2& from, const Vector2& to )
	{
		return abs( from.X - to.X ) + abs( from.Y - to.Y );
	}

	FORGE_INLINE Bool EndNodeCompareDefault( NodeID left, NodeID right )
	{
		return left == right;
	}

	template< class DataType >
	static void PerformAStar( NodeID startNode, NodeID endNode, const NavigationGraph< DataType >& graph, PathAsNodes& outPath, std::type_identity_t< std::function< Float( const DataType&, const DataType& ) > > heuristicFunc = &BasicHeuristicFormula, std::type_identity_t< std::function< Bool( NodeID, NodeID ) > > endNodeComparator = &EndNodeCompareDefault )
	{
		struct QueueNode
		{
			NodeID m_node = 0u;
			Float m_cost = 0.0f;
			Uint32 m_generation = 0u;
		};

		Uint32 nodesAmount = graph.GetNodesAmount();
		std::vector< QueueNode > queue;
		queue.reserve( nodesAmount );
		queue.push_back( { startNode, 0.0f } );

		std::vector< Uint32 > nodeGenerations;
		nodeGenerations.resize( nodesAmount, 0u );

		std::vector< AStarNodeData > nodesData;
		nodesData.resize( nodesAmount, { -1.0f, std::numeric_limits< Float >::infinity(), std::numeric_limits< Float >::infinity(), std::numeric_limits< NodeID >::max() } );

		nodesData[ startNode ].m_cost = 0.0f;
		nodesData[ startNode ].m_heuristicCost = heuristicFunc( graph.GetLocationFromID( startNode ), graph.GetLocationFromID( endNode ) );
		nodesData[ startNode ].m_localHeuristicCost = nodesData[ startNode ].m_heuristicCost;

		Bool succeeded = false;
		while ( !queue.empty() )
		{
			auto queuePredicate = []( const QueueNode& l, const QueueNode& r )
				{
					return l.m_cost > r.m_cost;
				};

			std::pop_heap( queue.begin(), queue.end(), queuePredicate );
			QueueNode node = queue.back();
			queue.pop_back();

			if ( nodeGenerations[ node.m_node ] > node.m_generation )
			{
				continue;
			}

			NodeID currentNode = node.m_node;
			if ( endNodeComparator( currentNode, endNode ) )
			{
				endNode = currentNode;
				succeeded = true;
				break;
			}

			AStarNodeData& currentNodeData = nodesData[ currentNode ];

			for ( const auto& connection : graph.GetNode( currentNode ).GetAllConnections() )
			{
				if ( !graph.GetNode( connection.m_to ).IsEnabled() )
				{
					continue;
				}

				AStarNodeData& destinationData = nodesData[ connection.m_to ];

				Float overallCost = currentNodeData.m_cost + connection.m_cost;

				if ( destinationData.m_cost > overallCost )
				{
					destinationData.m_cost = overallCost;
					destinationData.m_predecessor = currentNode;

					if ( destinationData.m_localHeuristicCost < 0.0f )
					{
						destinationData.m_localHeuristicCost = heuristicFunc( graph.GetLocationFromID( connection.m_to ), graph.GetLocationFromID( endNode ) );
					}

					destinationData.m_heuristicCost = overallCost + destinationData.m_localHeuristicCost;

					queue.push_back( { connection.m_to, destinationData.m_heuristicCost, ++nodeGenerations[ connection.m_to ] } );
					std::push_heap( queue.begin(), queue.end(), queuePredicate );
				}
			}
		}

		if ( !succeeded )
		{
			outPath = {};
			return;
		}

		NodeID currentNode = endNode;

		while ( currentNode != startNode )
		{
			outPath.emplace_back( currentNode );
			currentNode = nodesData[ currentNode ].m_predecessor;
		}

		outPath.emplace_back( currentNode );

		std::reverse( outPath.begin(), outPath.end() );
	}
}