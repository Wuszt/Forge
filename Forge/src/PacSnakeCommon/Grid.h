#pragma once

namespace pacsnake
{
	class GridPawnID
	{
	public:
		static GridPawnID GenerateID()
		{
			static Uint32 s_prevID = 0u;
			GridPawnID id;
			id.m_id = ++s_prevID;
			return id;
		}

	Bool operator==( const GridPawnID& ) const = default;

	private:
		Uint32 m_id = 0u;
	};

	struct GridPawn
	{
		GridPawn( const Vector2& pos, const Vector2& dir )
			: m_pos( pos )
			, m_dir( dir )
			, m_id( GridPawnID::GenerateID() )
		{}

		Vector2 m_pos;
		Vector2 m_dir;
		const GridPawnID m_id;
	};

	class Grid
	{
	public:
		Grid( Uint32 width, Uint32 height )
			: m_width( width )
			, m_height( height )
		{}

		GridPawnID AddPawn( const Vector2& pos, const Vector2& dir );
		GridPawn* GetPawn( GridPawnID id );

		Uint32 GetWidth() const
		{
			return m_width;
		}

		Uint32 GetHeight() const
		{
			return m_height;
		}

		void Update();

		struct Collision
		{
			GridPawnID m_first;
			GridPawnID m_second;
		};

		std::vector< Collision > FindCollisions();

	private:
		std::vector< GridPawn > m_pawns;
		Uint32 m_width = 0u;
		Uint32 m_height = 0u;
	};
}