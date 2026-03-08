#pragma once
#include "../GameEngine/ISystem.h"
#include "IDebuggable.h"

namespace forge::ai
{
	class NavmeshSystem : public systems::ISystem, public forge::IDebuggable
	{
		RTTI_DECLARE_CLASS( NavmeshSystem, systems::ISystem );

	protected:
		virtual void OnInitialize() override;

#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif

	private:
		void Generate( Float maxCellSize, Float agentRadius, Float agentHeight, Float maxSlopeAngle );

#ifdef FORGE_IMGUI_ENABLED
		struct DebugSettings
		{
			Float m_maxCellSize = 0.1f;
			Float m_agentRadius = 0.3f;
			Float m_agentHeight = 1.5f;
			Float m_maxStep = 0.35f;
		};

		DebugSettings m_debugSettings;
#endif
	};
}