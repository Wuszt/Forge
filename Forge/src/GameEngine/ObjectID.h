#pragma once

namespace forge
{
	struct ObjectID
	{
		ObjectID() = default;

		ObjectID( Uint32 id )
			: m_id( id )
		{}

		~ObjectID() = default;

		Uint32 m_id = 0u;

		static const Uint32 c_invalidID = 0u;

		Bool IsValid()
		{
			return m_id != c_invalidID;
		}

		Bool operator==( const ObjectID& id ) const
		{
			return m_id == id.m_id;
		}
	};
}

namespace std
{
	template<> struct std::hash<forge::ObjectID> {
		std::size_t operator()( const forge::ObjectID& s ) const noexcept
		{
			return s.m_id;
		}
	};
}