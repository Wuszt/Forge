#pragma once
#ifdef FORGE_IMGUI_ENABLED
#include "IDebuggable.h"
namespace ecs
{
	class ECSDebug : public forge::IDebuggable
	{
		DECLARE_CLASS( ECSDebug );
	public:
		ECSDebug();
		ECSDebug( forge::EngineInstance& engineInstance );
		ECSDebug( ECSDebug&& );
		~ECSDebug();

	private:
		void OnRenderDebug() override;

		forge::EngineInstance* m_engineInstance = nullptr;
	};
}
#endif

