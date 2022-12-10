#pragma once

namespace forge
{
	class EngineInstance;
}

namespace systems
{
	class ISystem
	{
		friend class SystemsManager;
		DECLARE_ABSTRACT_CLASS( ISystem );
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		forge::EngineInstance& GetEngineInstance() const
		{
			return *m_engineInstance;
		}

	protected:
		virtual void OnInitialize() {}
		virtual void OnDeinitialize() {}

	private:
		void Initialize( forge::EngineInstance& engineInstance );
		void Deinitialize();
		forge::EngineInstance* m_engineInstance;
	};
}

