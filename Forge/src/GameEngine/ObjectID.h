#pragma once

namespace forge
{
	class ObjectID
	{
		friend class ObjectsManager;

	public:
		static const Uint32 c_invalidID = std::numeric_limits< Uint32 >::max();

		ObjectID() : ObjectID( c_invalidID )
		{}

		~ObjectID() = default;

		Bool IsValid() const
		{
			return m_id != c_invalidID;
		}

		Bool operator==( const ObjectID& id ) const
		{
			return m_id == id.m_id;
		}

		explicit operator Uint32() const
		{
			return m_id;
		}

	private:
		explicit ObjectID( Uint32 id )
			: m_id( id )
		{}

		Uint32 m_id = c_invalidID;
	};
}

namespace std
{
	template<> struct std::hash<forge::ObjectID> {
		std::size_t operator()( const forge::ObjectID& s ) const noexcept
		{
			return static_cast< Uint32 >( s );
		}
	};
}