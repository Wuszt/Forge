#pragma once

namespace forge
{
	class EngineInstance;

	class ApplicationInstance
	{
	public:
		virtual void Initialize( EngineInstance& engineInstance ) {}
		virtual void OnUpdate( EngineInstance& engineInstance ) {}
		virtual void Deinitialize( EngineInstance& engineInstance ) {}

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
		Bool m_shutdownRequested = false;
	};
}
