#pragma once
#include "../GameEngine/ApplicationInstance.h"

namespace editor
{
	class IPanel;
}

namespace forge
{
	class EditorInstance : public forge::ApplicationInstance
	{
	public:
		EditorInstance( const std::string& applicationName );
		~EditorInstance();

		virtual void Initialize(forge::EngineInstance& engineInstance) override;
		virtual Bool WithRendering() const override
		{
			return true;
		}

	private:
		void Update();

		forge::CallbackToken m_updateToken;
		forge::EngineInstance* m_engineInstance = nullptr;

		std::vector<std::unique_ptr<editor::IPanel>> m_panels;
	};
}