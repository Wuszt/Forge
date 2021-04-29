#include "Fpch.h"
#include "PathFinding.h"

namespace AI
{
	std::vector< Path > PerformDijkstra( Graph::NodeID startNode, const Graph& graph )
	{
		const auto& nodes = graph.GetAllNodes();

		struct NodeData
		{
			Float m_cost;
			Graph::NodeID m_predecessor;
		};

		std::vector< NodeData > nodesData;
		nodesData.resize( nodes.size(), { std::numeric_limits< Float >::infinity(), std::numeric_limits< Uint32 >::max() } );

		nodesData[ startNode ].m_predecessor = startNode;
		nodesData[ startNode ].m_cost = 0.0f;

		std::vector< Graph::NodeID > queue;
		queue.reserve( nodes.size() );

		queue.emplace_back( startNode );

		std::vector< Bool > scoredNodes;
		scoredNodes.resize( nodes.size(), false );

		struct Pred
		{
			const std::vector< NodeData >& m_nodesData;

			bool operator () ( Float value, Graph::NodeID id ) const {
				return m_nodesData[ id ].m_cost < value;
			}

			bool operator () ( Graph::NodeID id , Float value ) const {
				return m_nodesData[ id ].m_cost > value;
			}
		};

		while( !queue.empty() )
		{
			auto minIt = queue.back();
			queue.pop_back();

			Graph::NodeID currentNode = minIt;
			const NodeData& currentNodeData = nodesData[ currentNode ];

			for( const auto& connection : graph.GetNode( currentNode ).GetAllConnections() )
			{

				NodeData& destinationData = nodesData[ connection.m_to ];

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

		std::vector< Path > paths;

		for( Uint32 i = 0; i < nodesData.size(); ++i )
		{
			Path path;

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

			paths.emplace_back( std::move( path ) );
		}

		return paths;
	}
}

