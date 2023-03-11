#pragma once
namespace ecs
{
	class EntityID
	{
		friend class ECSManager;

	public:
		EntityID() = default;
		~EntityID() = default;

		static const Uint32 c_invalidID = std::numeric_limits< Uint32 >::max();

		Bool IsValid()
		{
			return m_id != c_invalidID;
		}

		Bool operator==( const EntityID& id ) const
		{
			return m_id == id.m_id;
		}

		explicit operator Uint32() const
		{
			return m_id;
		}

	private:
		explicit EntityID( Uint32 id )
			: m_id( id )
		{}

		Uint32 m_id = c_invalidID;
	};
}

namespace std
{
	template<>
	struct std::hash< ecs::EntityID >
	{
		inline std::size_t operator()( const ecs::EntityID& s ) const noexcept
		{
			return static_cast< Uint32 >( s );
		}
	};
}