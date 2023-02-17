#pragma once
#include "IDebuggable.h"

namespace systems
{
	class AnimationSystem : public ISystem, public forge::IDebuggable
	{
		DECLARE_POLYMORPHIC_CLASS( AnimationSystem, systems::ISystem );

	public:
		virtual void OnInitialize() override;

	private:
		void Update();

		std::vector< Matrix > m_temporaryTransforms;
		forge::CallbackToken m_updateToken;
	protected:
#ifdef FORGE_IMGUI_ENABLED
		virtual void OnRenderDebug() override;
#endif
	};
}
