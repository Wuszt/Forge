#pragma once
namespace forge
{
	class IInput;
	class TransformComponent;

	class PlayerControllerComponent : public IComponent
	{
		DECLARE_POLYMORPHIC_CLASS( PlayerControllerComponent, forge::IComponent );
	public:
		using IComponent::IComponent;

		virtual void Update() {}
	};

	class FreeCameraControllerComponent : public PlayerControllerComponent
	{
	public:
		using PlayerControllerComponent::PlayerControllerComponent;

		virtual void Update() override;

	protected:
		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;

	private:
		IInput* m_input;
		TransformComponent* m_ownerTransform;

		Float m_currentSpeed = 3.0f;

		Vector3 m_eulerAngles;
	};
}

