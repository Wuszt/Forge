#pragma once

namespace forge
{
	class EngineInstance;

	class ApplicationInstance
	{
	public:
		ApplicationInstance( const std::string& applicationName );
		virtual ~ApplicationInstance() = default;
		virtual void Initialize( EngineInstance& engineInstance ) {}
		virtual void OnUpdate( EngineInstance& engineInstance ) {}
		virtual void Deinitialize( EngineInstance& engineInstance ) {}

		const std::string& GetApplicationName() const
		{
			return m_applicationName;
		}

		void Shutdown()
		{
			m_shutdownRequested = true;
		}

		virtual Bool WithRendering() const
		{
			return false;
		}

		virtual Bool ShouldShutdown() const
		{
			return m_shutdownRequested;
		}

	private:
		const std::string m_applicationName;
		Bool m_shutdownRequested = false;
	};
}
