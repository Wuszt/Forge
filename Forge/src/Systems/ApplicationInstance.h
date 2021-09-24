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

		virtual Bool WithRendering() const
		{
			return false;
		}

		virtual Bool ShouldShutdown() const
		{
			return false;
		}
	};
}
