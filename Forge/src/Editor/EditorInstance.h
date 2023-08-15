#pragma once
#include "../GameEngine/ApplicationInstance.h"

namespace forge
{
	class EditorInstance : public forge::ApplicationInstance
	{
	public:
		using forge::ApplicationInstance::ApplicationInstance;

		virtual void Initialize(forge::EngineInstance& engineInstance) override;
		virtual void OnUpdate(forge::EngineInstance& engineInstance) override;
		virtual Bool WithRendering() const override
		{
			return true;
		}
	};
}