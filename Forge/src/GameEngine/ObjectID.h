#pragma once

namespace forge
{
	class ObjectID
	{
	public:
		ObjectID() = default;

		~ObjectID() = default;

		static ObjectID GenerateNewId()
		{
			static Uint32 s_nextAvailableID = 1u;
			return ObjectID( s_nextAvailableID++ );
		}

		static const Uint32 c_invalidID = 0u;

		Bool IsValid()
		{
			return m_id != c_invalidID;
		}

		Bool operator==( const ObjectID& id ) const
		{
			return m_id == id.m_id;
		}

		Uint32 AsUint32() const
		{
			return m_id;
		}

	private:
		explicit ObjectID( Uint32 id )
			: m_id( id )
		{}

		Uint32 m_id = 0u;
	};
}

namespace std
{
	template<> struct std::hash<forge::ObjectID> {
		std::size_t operator()( const forge::ObjectID& s ) const noexcept
		{
			return s.AsUint32();
		}
	};
}