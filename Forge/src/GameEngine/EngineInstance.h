#pragma once

namespace renderer
{
	class RenderingManager;
}

namespace systems
{
	class SystemsManager;
}

namespace ecs
{
	class ECSManager;
}

namespace forge
{
	class ObjectsManager;
	class UpdateManager;
	class IWindow;
	class ApplicationInstance;
	class DepotsContainer;
	class AssetsManager;
	class SceneManager;

	class EngineInstance
	{
	public:
		EngineInstance( ApplicationInstance& gameInstance );
		EngineInstance( const EngineInstance& ei ) = delete;
		~EngineInstance();

		systems::SystemsManager& GetSystemsManager() const
		{
			FORGE_ASSERT( m_systemManager );
			return *m_systemManager;
		}

		ObjectsManager& GetObjectsManager() const
		{
			FORGE_ASSERT( m_objectsManager );
			return *m_objectsManager;
		}

		UpdateManager& GetUpdateManager() const
		{
			FORGE_ASSERT( m_updateManager );
			return *m_updateManager;
		}

		const DepotsContainer& GetDepotsContainer() const
		{
			FORGE_ASSERT( m_depotsContainer );
			return *m_depotsContainer;
		}

		AssetsManager& GetAssetsManager() const
		{
			FORGE_ASSERT( m_assetsManager );
			return *m_assetsManager;
		}

		ecs::ECSManager& GetECSManager() const
		{
			FORGE_ASSERT( m_ecsManager );
			return *m_ecsManager;
		}

		renderer::RenderingManager& GetRenderingManager() const
		{
			FORGE_ASSERT( m_renderingManager );
			return *m_renderingManager;
		}

		SceneManager& GetSceneManager() const
		{
			FORGE_ASSERT( m_sceneManager );
			return *m_sceneManager;
		}

		ApplicationInstance& GetApplicationInstance() const
		{
			return m_appInstance;
		}

		void Run();

	private:
		std::unique_ptr< ObjectsManager > m_objectsManager;
		std::unique_ptr< UpdateManager > m_updateManager;
		std::unique_ptr< DepotsContainer > m_depotsContainer;
		std::unique_ptr< AssetsManager > m_assetsManager;
		std::unique_ptr< ecs::ECSManager > m_ecsManager;
		std::unique_ptr< renderer::RenderingManager > m_renderingManager;
		std::unique_ptr< systems::SystemsManager > m_systemManager;
		std::unique_ptr< forge::SceneManager > m_sceneManager;

		forge::CallbackToken m_windowClosedToken;

		ApplicationInstance& m_appInstance;
	};
}

