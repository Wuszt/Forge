#pragma once
#include "../Core/Path.h"

namespace forge
{
	class Scene;
	class Stream;

	class SceneManager
	{
	public:
		SceneManager( forge::ObjectsManager& objectsManager );
		~SceneManager();

		void OpenEmptyScene();
		void SaveScene( forge::Path path );
		void SaveCurrentScene();
		void LoadScene( forge::Path path );

		Bool HasScenePath() const
		{
			return !m_currentScenePath.IsEmpty();
		}

		Scene& GetScene()
		{
			return *m_scene;
		}

		[[ nodiscard ]] forge::CallbackToken RegisterOnNewScene( forge::Callback<>::TFunc func )
		{
			return m_onNewScene.AddListener( std::move( func ) );
		}

	private:
		forge::Path m_currentScenePath;
		std::unique_ptr< Scene > m_scene;
		forge::ObjectsManager& m_objectsManager;
		forge::Callback<> m_onNewScene;
	};
}