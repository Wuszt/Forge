#pragma once

namespace forge
{
	class Scene;

	class SceneManager
	{
	public:
		SceneManager( forge::ObjectsManager& objectsManager );
		~SceneManager();

		Scene& GetScene()
		{
			return *m_scene;
		}

	private:
		std::unique_ptr< Scene > m_scene;
		forge::ObjectsManager& m_objectsManager;
	};
}