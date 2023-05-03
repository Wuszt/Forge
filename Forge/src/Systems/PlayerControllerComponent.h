#pragma once
namespace forge
{
	class IInput;
	class TransformComponent;
	class PhysicsDynamicComponent;

	class PlayerControllerComponent : public IComponent
	{
		DECLARE_POLYMORPHIC_CLASS( PlayerControllerComponent, forge::IComponent );
	public:
		using IComponent::IComponent;

		virtual void Update( bool isInputEnabled ) {}
	};

	class FreeCameraControllerComponent : public PlayerControllerComponent
	{
	public:
		using PlayerControllerComponent::PlayerControllerComponent;

		virtual void Update( bool isInputEnabled ) override;

	protected:
		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override;

	private:
		IInput* m_input = nullptr;
		TransformComponent* m_ownerTransform = nullptr;
		PhysicsDynamicComponent* m_physicsComponent = nullptr;

		Float m_currentSpeed = 3.0f;

		Vector3 m_eulerAngles;
	};
}

