#pragma once
#include "../Core/ApplicationArgs.h"

namespace forge
{
	class EngineInstance;

	class ApplicationInstance
	{
	public:
		ApplicationInstance( std::string applicationName, ApplicationArgs args = ApplicationArgs() );
		virtual ~ApplicationInstance() = default;
		virtual void Initialize( EngineInstance& engineInstance ) {}
		virtual void OnUpdate( EngineInstance& engineInstance ) {}
		virtual void Deinitialize( EngineInstance& engineInstance ) {}

		const std::string& GetApplicationName() const
		{
			return m_applicationName;
		}

		const ApplicationArgs& GetApplicationArgs() const
		{
			return m_applicationArgs;
		}

		void Shutdown()
		{
			m_shutdownRequested = true;
		}

		virtual Bool WithWindow() const
		{
			return false;
		}

		virtual Bool ShouldShutdown() const
		{
			return m_shutdownRequested;
		}

	private:
		ApplicationArgs m_applicationArgs;
		const std::string m_applicationName;
		Bool m_shutdownRequested = false;
	};
}
