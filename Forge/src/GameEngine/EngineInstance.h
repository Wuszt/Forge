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

	class EngineInstance
	{
	public:
		EngineInstance( ApplicationInstance& gameInstance );
		EngineInstance( const EngineInstance& ei ) = delete;
		~EngineInstance();

		systems::SystemsManager& GetSystemsManager() const
		{
			return *m_systemManager;
		}

		ObjectsManager& GetObjectsManager() const
		{
			return *m_objectsManager;
		}

		UpdateManager& GetUpdateManager() const
		{
			return *m_updateManager;
		}

		const DepotsContainer& GetDepotsContainer() const
		{
			return *m_depotsContainer;
		}

		AssetsManager& GetAssetsManager() const
		{
			return *m_assetsManager;
		}

		ecs::ECSManager& GetECSManager() const
		{
			return *m_ecsManager;
		}


		renderer::RenderingManager& GetRenderingManager() const
		{
			return *m_renderingManager;
		}

		void Run();

	private:
		std::unique_ptr< ObjectsManager > m_objectsManager;
		std::unique_ptr< systems::SystemsManager > m_systemManager;
		std::unique_ptr< UpdateManager > m_updateManager;
		std::unique_ptr< DepotsContainer > m_depotsContainer;
		std::unique_ptr< AssetsManager > m_assetsManager;
		std::unique_ptr< ecs::ECSManager > m_ecsManager;
		std::unique_ptr< renderer::RenderingManager > m_renderingManager;

		forge::CallbackToken m_windowClosedToken;

		ApplicationInstance& m_appInstance;
	};
}

