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
	static void GetPaths( const std::vector< DataType >& nodesData, std::vector< PathAsNodes >& outPaths )
	{
		for( Uint32 i = 0; i < nodesData.size(); ++i )
		{
			PathAsNodes path;

			if( nodesData[ i ].m_cost != std::numeric_limits< Float >::infinity() )
			{
				auto currentNode = i;

				while( nodesData[ currentNode ].m_cost > 0.0f )
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
	static void GetPaths( const GraphType& graph, const std::vector< DataType >& nodesData, std::vector< PathAsConnections >& outPaths )
	{
		for( Uint32 i = 0; i < nodesData.size(); ++i )
		{
			PathAsConnections path;

			if( nodesData[ i ].m_cost != std::numeric_limits< Float >::infinity() )
			{
				auto currentNode = i;

				while( nodesData[ currentNode ].m_cost > 0.0f )
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

		while( !queue.empty() )
		{
			auto minIt = queue.back();
			queue.pop_back();

			NodeID currentNode = minIt;
			const DijkstraNodeData& currentNodeData = nodesData[ currentNode ];

			for( const auto& connection : graph.GetNode( currentNode ).GetAllConnections() )
			{

				DijkstraNodeData& destinationData = nodesData[ connection.m_to ];

				Float overallCost = currentNodeData.m_cost + connection.m_cost;
				if( destinationData.m_cost > overallCost )
				{
					Float previousScore = destinationData.m_cost;

					if( scoredNodes[ connection.m_to ] )
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
		GetPaths( data, outPaths );
	}

	template< class GraphType >
	void PerformDijkstra( NodeID startNode, const GraphType& graph, std::vector< PathAsConnections >& outPaths )
	{
		std::vector< DijkstraNodeData > data = PerformDijkstraInternal( startNode, graph );
		GetPaths( graph, data, outPaths );
	}

	template< class DataType >
	Float BasicHeuristicFormula( const DataType& from, const DataType& to )
	{
		return from.DistanceSquaredTo( to );
	}

	Float ManhattanHeuristicFormula( const Vector2& from, const Vector2& to );

	template< class DataType >
	static void PerformAStar( NodeID startNode, NodeID endNode, const NavigationGraph< DataType >& graph, PathAsNodes& outPath, Float( *heuristicFunc )( const DataType&, const DataType& ) = &BasicHeuristicFormula )
	{
		Uint32 nodesAmount = graph.GetNodesAmount();

		std::vector< NodeID > queue;
		queue.reserve( nodesAmount );

		queue.emplace_back( startNode );

		std::vector< Bool > scoredNodes;
		scoredNodes.resize( nodesAmount, false );

		std::vector< AStarNodeData > nodesData;
		nodesData.resize( nodesAmount, { -1.0f, std::numeric_limits< Float >::infinity(), std::numeric_limits< Float >::infinity(), std::numeric_limits< NodeID >::max() } );

		nodesData[ startNode ].m_cost = 0.0f;
		nodesData[ startNode ].m_heuristicCost = heuristicFunc( graph.GetLocationFromID( startNode ), graph.GetLocationFromID( endNode ) );
		nodesData[ startNode ].m_localHeuristicCost = nodesData[ startNode ].m_heuristicCost;

		while( !queue.empty() )
		{
			auto it = std::min_element( queue.begin(), queue.end(), [ &nodesData ]( const auto& l, const auto& r ) { return nodesData[ l ].m_heuristicCost < nodesData[ r ].m_heuristicCost; } );
			NodeID currentNode = *it;
			if( currentNode == endNode )
			{
				break;
			}

			queue.erase( it );

			scoredNodes[ currentNode ] = false;

			AStarNodeData& currentNodeData = nodesData[ currentNode ];

			struct Pred
			{
				const std::vector< AStarNodeData >& m_nodesData;

				bool operator () ( Float value, NodeID id ) const {
					return m_nodesData[ id ].m_heuristicCost < value;
				}

				bool operator () ( NodeID id, Float value ) const {
					return m_nodesData[ id ].m_heuristicCost > value;
				}
			};

			for( const auto& connection : graph.GetNode( currentNode ).GetAllConnections() )
			{
				AStarNodeData& destinationData = nodesData[ connection.m_to ];

				Float overallCost = currentNodeData.m_cost + connection.m_cost;

				if( destinationData.m_cost > overallCost )
				{
					if( scoredNodes[ connection.m_to ] )
					{
						auto pairIt = std::equal_range( queue.begin(), queue.end(), destinationData.m_heuristicCost, Pred{ nodesData } );

						auto it = std::find( pairIt.first, pairIt.second, connection.m_to );

						queue.erase( it );
					}

					destinationData.m_cost = overallCost;
					destinationData.m_predecessor = currentNode;

					if( destinationData.m_localHeuristicCost < 0.0f )
					{
						destinationData.m_localHeuristicCost = heuristicFunc( graph.GetLocationFromID( connection.m_to ), graph.GetLocationFromID( endNode ) );
					}

					destinationData.m_heuristicCost = overallCost + destinationData.m_localHeuristicCost;

					auto it = std::upper_bound( queue.begin(), queue.end(), destinationData.m_heuristicCost, Pred{ nodesData } );
					queue.emplace( it, connection.m_to );

					scoredNodes[ connection.m_to ] = true;
				}
			}
		}

		NodeID currentNode = endNode;
		
		while( currentNode != startNode )
		{
			outPath.emplace_back( currentNode );
			currentNode = nodesData[ currentNode ].m_predecessor;
		}
		
		outPath.emplace_back( currentNode );

		std::reverse( outPath.begin(), outPath.end() );
	}
}