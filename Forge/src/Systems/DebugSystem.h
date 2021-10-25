#pragma once
#ifdef FORGE_DEBUGGING

namespace systems
{
	class DebugSystem : public ISystem
	{
	public:
		using ISystem::ISystem;

		virtual void OnInitialize() override;

	private:
#ifdef FORGE_IMGUI_ENABLED
		std::unique_ptr< forge::CallbackToken > m_fpsCounterDrawingToken;
#endif
	};
}

#endif
