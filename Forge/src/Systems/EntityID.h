#pragma once

namespace forge
{
	struct EntityID
	{
		EntityID() = default;

		EntityID( Uint32 id )
			: m_id( id )
		{}

		~EntityID() = default;

		Uint32 m_id = 0u;

		static const Uint32 c_invalidID = 0u;

		FORGE_INLINE Bool IsValid()
		{
			return m_id != c_invalidID;
		}

		FORGE_INLINE Bool operator==( const EntityID& id ) const
		{
			return m_id == id.m_id;
		}
	};
}

namespace std
{
	template<> struct std::hash<forge::EntityID> {
		std::size_t operator()( forge::EntityID const& s ) const noexcept
		{
			return s.m_id;
		}
	};
}