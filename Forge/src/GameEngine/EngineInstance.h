#pragma once

namespace renderer
{
	class IRenderer;
}

namespace systems
{
	class SystemsManager;
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

		renderer::IRenderer& GetRenderer() const
		{
			return *m_renderer;
		}

		IWindow& GetWindow() const
		{
			return *m_window;
		}

		const DepotsContainer& GetDepotsContainer() const
		{
			return *m_depotsContainer;
		}

		AssetsManager& GetAssetsManager() const
		{
			return *m_assetsManager;
		}

		void Run();

	private:
		std::unique_ptr< ObjectsManager > m_objectsManager;
		std::unique_ptr< systems::SystemsManager > m_systemManager;
		std::unique_ptr< UpdateManager > m_updateManager;
		std::unique_ptr< DepotsContainer > m_depotsContainer;
		std::unique_ptr< AssetsManager > m_assetsManager;

		std::unique_ptr< renderer::IRenderer > m_renderer;
		std::unique_ptr< IWindow > m_window;

		forge::CallbackToken m_windowUpdateToken;
		forge::CallbackToken m_windowClosedToken;

		ApplicationInstance& m_appInstance;
	};

	class IManager
	{
	public:
		IManager( EngineInstance& engineInstance )
			: m_engineInstance( engineInstance )
		{}

		IManager( const IManager& manager ) = delete;

		virtual void Initialize() {}
		virtual void Deinitialize() {}

		EngineInstance& GetEngineInstance() const
		{
			return m_engineInstance;
		}

	protected:
		virtual ~IManager() = default;

		EngineInstance& m_engineInstance;
	};

}

