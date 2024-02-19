#pragma once
namespace forge
{
	class IInput;
	class TransformComponent;
	class PhysicsDynamicComponent;

	class IPlayerControllerComponent : public IComponent
	{
		RTTI_DECLARE_ABSTRACT_CLASS( IPlayerControllerComponent, forge::IComponent );
	public:
		using IComponent::IComponent;

		virtual void Update( Bool isInputEnabled ) = 0;
	};

	class PhysicsFreeCameraControllerComponent : public IPlayerControllerComponent
	{
	public:
		using IPlayerControllerComponent::IPlayerControllerComponent;

		virtual void Update( Bool isInputEnabled ) override;

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

