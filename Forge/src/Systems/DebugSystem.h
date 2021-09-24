#pragma once
namespace systems
{
	class DebugSystem : public ISystem
	{
	public:
		using ISystem::ISystem;

		virtual void OnInitialize() override;

	private:
		std::unique_ptr< forge::CallbackToken > m_fpsCounterDrawingToken;
	};
}

