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

		void Reset()
		{
			*this = ObjectLifetimeToken();
		}

		forge::ObjectID GetObjectId() const
		{
			return m_objectId;
		}

		Bool IsValid() const
		{
			return m_objectId.IsValid();
		}

		template< class T = forge::Object >
		T* GetObject() const
		{
			if ( !m_objectId.IsValid() )
			{
				return nullptr;
			}

			return dynamic_cast< T* >( m_objectsManager->GetObject( m_objectId ) );
		}

	private:
		ObjectsManager* m_objectsManager = nullptr;
		forge::ObjectID m_objectId;
	};
}