#pragma once
namespace forge
{
	class IInput;
	class TransformComponent;

	class PlayerControllerComponent : public IComponent
	{
	public:
		using IComponent::IComponent;

		virtual void Update() {}
	};

	class FreeCameraControllerComponent : public PlayerControllerComponent
	{
	public:
		using PlayerControllerComponent::PlayerControllerComponent;

		virtual void OnAttach( EngineInstance& engineInstance );

		virtual void Update() override;

	private:
		IInput* m_input;
		TransformComponent* m_ownerTransform;

		Float m_currentSpeed = 3.0f;

		Vector3 m_eulerAngles;
	};
}

