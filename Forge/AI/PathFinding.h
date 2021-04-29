#pragma once
#include "Graph.h"

namespace AI
{
	using Path = std::vector< Graph::NodeID >;

	std::vector< Path > AI_API PerformDijkstra( Graph::NodeID startNode, const Graph& graph );
}