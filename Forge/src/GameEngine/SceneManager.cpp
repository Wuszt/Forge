#include "Fpch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "../Core/Path.h"
#include "../Core/Streams.h"

forge::SceneManager::SceneManager( forge::ObjectsManager& objectsManager, const ApplicationArgs& applicationArgs )
	: m_objectsManager( objectsManager )
{
	if ( const std::string* sceneRawPath = applicationArgs.GetArgumentValue( "StartupLevel" ) )
	{
		forge::Path scenePath( *sceneRawPath );
		if ( !scenePath.IsEmpty() )
		{
			LoadScene( scenePath );
			return;
		}
	}

	m_scene = std::make_unique< Scene >( m_objectsManager );
}

forge::SceneManager::~SceneManager() = default;

void forge::SceneManager::OpenEmptyScene()
{
	m_currentScenePath = forge::Path();
	m_scene = std::make_unique< Scene >( m_objectsManager );
	m_onNewScene.Invoke();
}

void forge::SceneManager::SaveScene( forge::Path path )
{
	forge::FileStream stream( path, false );
	m_currentScenePath = std::move( path );
	GetScene().Serialize( stream );
}

void forge::SceneManager::SaveCurrentScene()
{
	if ( HasScenePath() )
	{
		SaveScene( m_currentScenePath );
	}
}

void forge::SceneManager::LoadScene( forge::Path path )
{
	forge::FileStream stream( path, false );
	m_currentScenePath = std::move( path );
	m_scene = std::make_unique< Scene >( m_objectsManager );
	GetScene().Deserialize( stream );
	m_onNewScene.Invoke();
}