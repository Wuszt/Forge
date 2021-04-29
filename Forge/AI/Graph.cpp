#include "Fpch.h"
#include "Graph.h"

using namespace AI;

Graph::Graph()
{}

Graph::~Graph()
{}

Graph::NodeID Graph::AddNode( const Node& node )
{
	m_nodes.emplace_back( node );

	return static_cast< Uint32 >( m_nodes.size() - 1 );
}

Graph::NodeID AI::Graph::AddEmptyNode()
{
	return AddNode( Node() );
}

const AI::Graph::Node& AI::Graph::GetNode( const NodeID& id ) const
{
	return m_nodes[ id ];
}

Graph::Node& Graph::GetNode( const NodeID& id )
{
	return m_nodes[ id ];
}

Graph::Node::Node()
{}

Graph::Node::~Node()
{}

void Graph::Node::AddConnection( const Connection& connection )
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
	else
	{
		*it = connection;
	}
}

Bool Graph::Node::RemoveConnection( const NodeID& destination )
{
	auto it = std::find_if( m_connections.begin(), m_connections.end(), [ &destination ]( const Connection& element ) { return element.m_to == destination; } );

	if( it != m_connections.end() )
	{
		m_connections.erase( it );
		return true;
	}

	return false;
}

void Graph::AddConnection( const NodeID& from, const NodeID& to, Float cost )
{
	m_nodes[ from ].AddConnection( { to, cost } );
}