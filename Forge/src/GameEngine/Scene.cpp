#include "Fpch.h"
#include "Scene.h"

forge::Scene::~Scene()
{
	for ( forge::ObjectID objectID : m_objects )
	{
		m_objectsManager.RequestDestructingObject( objectID );
	}
}
