#include "Fpch.h"
#include "Grid.h"

pacsnake::GridPawnID pacsnake::Grid::AddPawn( const Vector2& pos, const Vector2& dir )
{
	return m_pawns.emplace_back( pos, dir ).m_id;
}

pacsnake::GridPawn* pacsnake::Grid::GetPawn( GridPawnID id )
{
	auto it = std::find_if( m_pawns.begin(), m_pawns.end(), [ id ]( const GridPawn& pawn ) { return pawn.m_id == id; } );
	if ( it != m_pawns.end() )
	{
		return &*it;
	}

	return nullptr;
}

void pacsnake::Grid::Update()
{
	for ( GridPawn& pawn : m_pawns )
	{
		pawn.m_pos += pawn.m_dir;
	}
}

std::vector< pacsnake::Grid::Collision > pacsnake::Grid::FindCollisions()
{
	std::vector< pacsnake::Grid::Collision > collisions;

	for ( Uint32 i = 0u; i < m_pawns.size(); ++i )
	{
		for ( Uint32 j = i + 1; j < m_pawns.size(); ++j )
		{
			if ( m_pawns[ i ].m_pos == m_pawns[ j ].m_pos )
			{
				collisions.push_back( { m_pawns[ i ].m_id, m_pawns[ j ].m_id } );
			}
		}
	}

	return collisions;
}
