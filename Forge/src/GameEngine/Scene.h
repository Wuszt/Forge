#pragma once

namespace forge
{
	class Stream;

	class Scene
	{
	public:
		Scene( forge::ObjectsManager& objectsManager )
			: m_objectsManager( objectsManager )
		{}

		~Scene();

		void AttachToScene( ObjectID objectID )
		{
			m_objects.emplace( objectID );
		}

		void DetachFromScene( ObjectID objectID )
		{
			m_objects.erase( objectID );
		}

		template< class Visitor >
		void VisitSceneObjects( Visitor&& visitor )
		{
			for ( forge::ObjectID objectID : m_objects )
			{
				visitor( objectID );
			}
		}

		void Serialize( forge::Stream& stream ) const;
		void Deserialize( forge::Stream& stream );

	private:
		std::unordered_set< forge::ObjectID > m_objects;
		forge::ObjectsManager& m_objectsManager;
	};
}