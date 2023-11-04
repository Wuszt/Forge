#pragma once

#include "ObjectID.h"

namespace forge
{
	class Object;
	class ObjectsManager;

	class ObjectLifetimeToken
	{
	public:
		ObjectLifetimeToken() = default;
		ObjectLifetimeToken( ObjectID id, ObjectsManager& objectsManager );
		ObjectLifetimeToken( const Object& object );
		ObjectLifetimeToken( ObjectLifetimeToken&& other );
		~ObjectLifetimeToken();

		ObjectLifetimeToken& operator=( ObjectLifetimeToken&& other );

		forge::ObjectID GetObjectId() const
		{
			return m_objectId;
		}

		template< class T >
		T* GetObject() const
		{
			return dynamic_cast< T* >( m_objectsManager->GetObject( m_objectId ) );
		}

		template<>
		forge::Object* GetObject< forge::Object >() const
		{
			return m_objectsManager->GetObject( m_objectId );
		}

	private:
		ObjectsManager* m_objectsManager = nullptr;
		forge::ObjectID m_objectId;
	};
}