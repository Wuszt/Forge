#pragma once
#include "Graph.h"

namespace forge::ai
{
	namespace internal
	{
		struct PathFindingNodeData
		{
			Float m_cost = std::numeric_limits< Float >::infinity();
			ai::NodeID m_predecessor;
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

	namespace internal
	{
		template< class GraphType >
		static std::vector< PathFindingNodeData > PerformDijkstra( NodeID startNode, const GraphType& graph )
		{
			Uint32 nodesAmount = graph.GetNodesAmount();

			std::vector< PathFindingNodeData > nodesData;
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
				const std::vector< PathFindingNodeData >& m_nodesData;

				bool operator () ( Float value, NodeID id ) const {
					return m_nodesData[ id.Get() ].m_cost < value;
				}

				bool operator () ( NodeID id, Float value ) const {
					return m_nodesData[ id.Get() ].m_cost > value;
				}
			};

			while ( !queue.empty() )
			{
				auto minIt = queue.back();
				queue.pop_back();

				NodeID currentNode = minIt;
				const PathFindingNodeData& currentNodeData = nodesData[ currentNode ];

				for ( const auto& connection : graph.GetNode( currentNode ).GetAllConnections() )
				{

					PathFindingNodeData& destinationData = nodesData[ connection.m_to ];

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

		FORGE_INLINE Bool IsNodeAccessibleDefault( NodeID node, Float currentCost )
		{
			return true;
		}
	}

	template< class GraphType >
	void FindPath_Dijkstra( NodeID startNode, const GraphType& graph, std::vector< PathAsNodes >& outPaths )
	{
		std::vector< internal::PathFindingNodeData > data = PerformDijkstra( startNode, graph );
		GetPaths( forge::ArraySpan< const internal::PathFindingNodeData >( data ), outPaths );
	}

	template< class GraphType >
	void FindPath_Dijkstra( NodeID startNode, const GraphType& graph, std::vector< PathAsConnections >& outPaths )
	{
		std::vector< internal::PathFindingNodeData > data = PerformDijkstra( startNode, graph );
		GetPaths( graph, forge::ArraySpan< const internal::PathFindingNodeData >( data ), outPaths );
	}

	template< class DataType >
	Float EuclideanHeuristicFormula( const DataType& from, const DataType& to )
	{
		return from.DistanceSquaredTo( to );
	}

	FORGE_INLINE Float ManhattanHeuristicFormula( const Vector2& from, const Vector2& to )
	{
		return abs( from.X - to.X ) + abs( from.Y - to.Y );
	}

	template< class DataType >
	using HeuristicFunc = std::type_identity_t< std::function< Float( const DataType&, const DataType& ) > >;

	using NodeAccesibilityFunc = std::function< Bool( NodeID, Float ) >;

	template< class TDataType, Bool CheckEnablement = true, 
		class HeuristicFunc = forge::Functor< Float, const DataType&, const DataType& >::Static< ::EuclideanHeuristicFormula >,
		class NodeAccessibilityFunc = forge::Functor< Bool, NodeID, Float >::Static< internal::IsNodeAccessibleDefault >,
		Bool PreCalculateHeuristics = false
	>
	struct AStarConfig
	{
		using DataType = TDataType;
		static constexpr Bool c_checkEnablement = CheckEnablement;
		static constexpr Bool c_preCalculateHeuristics = PreCalculateHeuristics;

		AStarConfig( const NavigationGraph< DataType >& graph, HeuristicFunc heuristic = {}, NodeAccessibilityFunc accessibilityFunc = {}, Uint32 minAllowedTimeToRevisit = std::numeric_limits< Uint32 >::max() )
			: m_graph( graph )
			, m_heuristicFunc( std::move( heuristic ) )
			, m_nodeAccesibilityFunc( std::move( accessibilityFunc ) )
			, m_minAllowedTimeToRevisit( minAllowedTimeToRevisit )
		{}

		const NavigationGraph< DataType >& m_graph;
		HeuristicFunc m_heuristicFunc;
		NodeAccessibilityFunc m_nodeAccesibilityFunc;
		Uint32 m_minAllowedTimeToRevisit = std::numeric_limits< Uint32 >::max();
	};

	template< class TConfig >
	static void FindPath_AStar( forge::ArraySpan< const NodeID > startNodes, NodeID endNode, const TConfig& config, PathAsNodes& outPath )
	{
		PC_SCOPE_FUNC();

		struct QueueNode
		{
			NodeID m_node;
			Float m_cost = 0.0f;
			Uint32 m_generation = 0u;
		};

		auto& graph = config.m_graph;
		Uint32 nodesAmount = graph.GetNodesAmount();

		std::vector< Float > heuristics;
		heuristics.resize( nodesAmount, -1.0f );

		if constexpr ( config.c_preCalculateHeuristics )
		{
			for ( Uint32 i = 0u; i < nodesAmount; ++i )
			{
				heuristics[ i ] = config.m_heuristicFunc( graph.GetLocationFromID( NodeID( i ) ), graph.GetLocationFromID( endNode ) );
			}
		}

		std::vector< Uint32 > nodeGenerations;
		nodeGenerations.resize( nodesAmount, 0u );

		std::vector< QueueNode > queue;
		queue.reserve( nodesAmount );

		std::vector< internal::PathFindingNodeData > nodesData;
		nodesData.resize( nodesAmount );

		for ( const NodeID startNode : startNodes )
		{
			queue.push_back( { startNode, 0.0f } );
			internal::PathFindingNodeData& startData = nodesData[ startNode.Get() ];
			startData.m_cost = 0.0f;
		}

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

			NodeID currentNode = node.m_node;
			if ( nodeGenerations[ currentNode.Get() ] > node.m_generation )
			{
				continue;
			}

			if ( currentNode == endNode )
			{
				endNode = currentNode;
				succeeded = true;
				break;
			}

			internal::PathFindingNodeData& currentNodeData = nodesData[ currentNode.Get() ];

			for ( const auto& connection : graph.GetNode( currentNode ).GetConnections() )
			{
				if constexpr ( config.c_checkEnablement )
				{
					if ( !graph.GetNode( connection.m_to ).IsEnabled() )
					{
						continue;
					}
				}

				if ( !config.m_nodeAccesibilityFunc( connection.m_to, currentNodeData.m_cost + connection.m_cost ) )
				{
					continue;
				}

				internal::PathFindingNodeData& destinationData = nodesData[ connection.m_to.Get() ];

				Float overallCost = currentNodeData.m_cost + connection.m_cost;

				if ( destinationData.m_cost > overallCost )
				{
					destinationData.m_cost = overallCost;
					destinationData.m_predecessor = currentNode;

					if constexpr ( !config.c_preCalculateHeuristics )
					{
						if ( heuristics[ connection.m_to.Get() ] < 0.0f )
						{
							heuristics[ connection.m_to.Get() ] = config.m_heuristicFunc( graph.GetLocationFromID( connection.m_to ), graph.GetLocationFromID( endNode ) );
						}
					}

					const Float heuristicCost = overallCost + heuristics[ connection.m_to.Get() ];

					queue.push_back( { connection.m_to, heuristicCost, ++nodeGenerations[ connection.m_to.Get() ] } );
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
		outPath.emplace_back( currentNode );

		while ( nodesData[ currentNode.Get() ].m_predecessor.IsValid() )
		{
			currentNode = nodesData[ currentNode.Get() ].m_predecessor;
			outPath.emplace_back( currentNode );
		}	

		std::reverse( outPath.begin(), outPath.end() );
	}

	using ShouldInterruptFunc = std::function< Bool() >;

	Bool ShouldInterruptDefault()
	{
		return false;
	}

	template< class TConfig >
	static forge::co::Generator< PathAsNodes > FindPaths_AStar( forge::ArraySpan< const NodeID > startNodes, NodeID endNode, const TConfig& config, ShouldInterruptFunc shouldInterruptFunc = &ShouldInterruptDefault )
	{
		PC_SCOPE_FUNC();

		struct QueueNode
		{
			Uint32 m_walkthroughNodeIndex = std::numeric_limits< Uint32 >::max();
			Float m_heuristic = std::numeric_limits< Float >::infinity();
		};

		struct WalkthroughNode
		{
			NodeID m_nodeID;
			Uint32 m_predecessor = std::numeric_limits< Uint32 >::max();
			Float m_cost = std::numeric_limits< Float >::infinity();
		};

		auto& graph = config.m_graph;
		const Uint32 nodesAmount = graph.GetNodesAmount();

		std::vector< WalkthroughNode > walkthroughNodes;
		walkthroughNodes.reserve( nodesAmount * 64u );

		std::vector< QueueNode > queue;
		queue.reserve( nodesAmount * 32u );

		std::vector< Float > heuristics;
		heuristics.resize( nodesAmount, -1.0f );

		if constexpr ( config.c_preCalculateHeuristics )
		{
			for ( Uint32 i = 0u; i < nodesAmount; ++i )
			{
				heuristics[ i ] = config.m_heuristicFunc( graph.GetLocationFromID( NodeID( i ) ), graph.GetLocationFromID( endNode ) );
			}
		}

		for ( const NodeID startNode : startNodes )
		{
			walkthroughNodes.push_back( { startNode, std::numeric_limits< Uint32 >::max(), 0.0f } );
			
			QueueNode newQueueNode;
			newQueueNode.m_walkthroughNodeIndex = static_cast< Uint32 >( walkthroughNodes.size() - 1u );
			newQueueNode.m_heuristic = config.m_heuristicFunc( graph.GetLocationFromID( startNode ), graph.GetLocationFromID( endNode ) );
			queue.push_back( newQueueNode );
		}

		auto queuePredicate = []( const QueueNode& l, const QueueNode& r )
			{
				return l.m_heuristic > r.m_heuristic || ( l.m_heuristic == r.m_heuristic && r.m_walkthroughNodeIndex > l.m_walkthroughNodeIndex );
			};

		while ( !queue.empty() )
		{
			if ( shouldInterruptFunc() )
			{
				co_return;
			}

			std::pop_heap( queue.begin(), queue.end(), queuePredicate );
			const QueueNode node = queue.back();
			queue.pop_back();

			const WalkthroughNode currentWalkthroughNode = walkthroughNodes[ node.m_walkthroughNodeIndex ];
			const NodeID currentNode = currentWalkthroughNode.m_nodeID;
			if ( currentNode == endNode )
			{
				PathAsNodes outPath;
				WalkthroughNode node = currentWalkthroughNode;
				
				outPath.emplace_back( node.m_nodeID );
				while ( node.m_predecessor != std::numeric_limits< Uint32 >::max() )
				{
					node = walkthroughNodes[ node.m_predecessor ];
					outPath.emplace_back( node.m_nodeID );
				}

				std::reverse( outPath.begin(), outPath.end() );
				co_yield std::move( outPath );
				continue;
			}

			{
				Bool containsNodeOnPath = false;
				Uint32 nodesAmountOnPath = 0u;
				Uint32 currNodeIndex = currentWalkthroughNode.m_predecessor;
				while ( currNodeIndex != std::numeric_limits< Uint32 >::max() && !containsNodeOnPath && nodesAmountOnPath < config.m_minAllowedTimeToRevisit )
				{
					const WalkthroughNode walkthroughNode = walkthroughNodes[ currNodeIndex ];
					containsNodeOnPath = walkthroughNode.m_nodeID == currentWalkthroughNode.m_nodeID;
					currNodeIndex = walkthroughNode.m_predecessor;
					++nodesAmountOnPath;
				}

				if ( containsNodeOnPath )
				{
					continue;
				}
			}

			for ( const auto& connection : graph.GetNode( currentNode ).GetConnections() )
			{
				if constexpr ( config.c_checkEnablement )
				{
					if ( !graph.GetNode( connection.m_to ).IsEnabled() )
					{
						continue;
					}
				}

				if ( !config.m_nodeAccesibilityFunc( connection.m_to, currentWalkthroughNode.m_cost + connection.m_cost ) )
				{
					continue;
				}

				if ( config.m_minAllowedTimeToRevisit > 0u
					&& currentWalkthroughNode.m_predecessor != std::numeric_limits< Uint32 >::max()
					&& walkthroughNodes[ currentWalkthroughNode.m_predecessor ].m_nodeID == connection.m_to )
				{
					continue;
				}

				const Float overallCost = currentWalkthroughNode.m_cost + connection.m_cost;

				WalkthroughNode newNode;
				newNode.m_nodeID = connection.m_to;
				newNode.m_predecessor = node.m_walkthroughNodeIndex;
				newNode.m_cost = overallCost;
				walkthroughNodes.emplace_back( newNode );

				if constexpr ( !config.c_preCalculateHeuristics )
				{
					if ( heuristics[ connection.m_to.Get() ] < 0.0f )
					{
						heuristics[ connection.m_to.Get() ] = config.m_heuristicFunc( graph.GetLocationFromID( connection.m_to ), graph.GetLocationFromID( endNode ) );
					}
				}

				QueueNode newQueueNode;
				newQueueNode.m_heuristic = overallCost + heuristics[ connection.m_to.Get() ];
				newQueueNode.m_walkthroughNodeIndex = static_cast< Uint32 >( walkthroughNodes.size() - 1u );

				queue.emplace_back( newQueueNode );
				std::push_heap( queue.begin(), queue.end(), queuePredicate );
			}
		}
	}
}