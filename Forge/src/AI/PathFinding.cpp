#include "Fpch.h"
#include "PathFinding.h"

Float AI::ManhattanHeuristicFormula( const Vector2& from, const Vector2& to )
{
	return abs( from.X - to.X ) + abs( from.Y - to.Y );
}

