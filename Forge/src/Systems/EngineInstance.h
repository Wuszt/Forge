#pragma once
#include "../Core/DepotsContainer.h"

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
	class EntitiesManager;
	class UpdateManager;
	class IWindow;
	class ApplicationInstance;

	class EngineInstance
	{
	public:
		EngineInstance( ApplicationInstance& gameInstance );
		EngineInstance( const EngineInstance& ei ) = delete;
		~EngineInstance();

		FORGE_INLINE systems::SystemsManager& GetSystemsManager() const
		{
			return *m_systemManager;
		}

		FORGE_INLINE EntitiesManager& GetEntitiesManager() const
		{
			return *m_entitiesManager;
		}

		FORGE_INLINE UpdateManager& GetUpdateManager() const
		{
			return *m_updateManager;
		}

		FORGE_INLINE renderer::IRenderer& GetRenderer() const
		{
			return *m_renderer;
		}

		FORGE_INLINE IWindow& GetWindow() const
		{
			return *m_window;
		}

		FORGE_INLINE const DepotsContainer& GetDepotsContainer() const
		{
			return m_depotsContainer;
		}

		void Run();

	private:
		std::unique_ptr< EntitiesManager > m_entitiesManager;
		std::unique_ptr< systems::SystemsManager > m_systemManager;
		std::unique_ptr< UpdateManager > m_updateManager;
		const DepotsContainer m_depotsContainer;

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

		FORGE_INLINE EngineInstance& GetEngineInstance() const
		{
			return m_engineInstance;
		}

	protected:
		virtual ~IManager() = default;

		EngineInstance& m_engineInstance;
	};

}

