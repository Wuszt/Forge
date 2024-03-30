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
		RTTI_DECLARE_ABSTRACT_CLASS( ISystem );
	public:
		virtual ~ISystem() = 0;

		forge::EngineInstance& GetEngineInstance() const
		{
			return *m_engineInstance;
		}

	protected:
		virtual void OnInitialize() {}
		virtual void OnDeinitialize() {}

		virtual void OnPostInit() {}

	private:
		void Initialize( forge::EngineInstance& engineInstance );
		void Deinitialize();

		void PostInit() 
		{
			OnPostInit();
		}

		forge::EngineInstance* m_engineInstance;
	};
}

