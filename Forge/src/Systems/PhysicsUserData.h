#pragma once

namespace physics
{
	struct UserData
	{
		UserData( ecs::EntityID entity, forge::ObjectID object )
			: m_entityId( entity )
			, m_objectId( object )
		{}

		UserData( void* rawUserData );
		void* GetAsPointerSizeType() const;

		ecs::EntityID m_entityId;
		forge::ObjectID m_objectId;
	};
}