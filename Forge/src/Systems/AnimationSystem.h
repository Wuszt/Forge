#pragma once
namespace systems
{
	class AnimationSystem : public ISystem
	{
		DECLARE_POLYMORPHIC_CLASS( AnimationSystem, systems::ISystem );

	public:
		virtual void OnInitialize() override;

	private:
		void Update();

		std::vector< Matrix > m_temporaryTransforms;
		forge::CallbackToken m_updateToken;
	};
}
