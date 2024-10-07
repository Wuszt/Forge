#include "Fpch.h"
#include "SceneManager.h"
#include "Scene.h"

forge::SceneManager::SceneManager( forge::ObjectsManager& objectsManager )
	: m_objectsManager( objectsManager )
{
	m_scene = std::make_unique< Scene >( m_objectsManager );
}

forge::SceneManager::~SceneManager() = default;
