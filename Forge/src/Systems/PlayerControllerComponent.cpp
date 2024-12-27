#include "Fpch.h"
#include "PlayerControllerComponent.h"
#include "../Core/IWindow.h"
#include "../Core/IInput.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsSystem.h"
#include "../Physics/PhysicsShape.h"
#include "../GameEngine/RenderingManager.h"
#include "PlayerSystem.h"
#include "../Systems/InputSystem.h"

RTTI_IMPLEMENT_TYPE( forge::IPlayerControllerComponent);

void forge::PhysicsFreeCameraControllerComponent::OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData )
{
	m_ownerTransform = GetOwner().GetComponent< forge::TransformComponent >();

	GetOwner().AddComponents< forge::PhysicsDynamicComponent >();
	m_physicsComponent = GetOwner().GetComponent< forge::PhysicsDynamicComponent >();
	//m_physicsComponent->AddShape( physics::PhysicsShape( GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< systems::PhysicsSystem >().GetPhysicsProxy(), 0.5f ) );
	m_physicsComponent->GetActor().EnableGravity( false );
}

void forge::PhysicsFreeCameraControllerComponent::Update()
{
	const forge::InputHandler& inputHandler = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< systems::PlayerSystem >().GetInputHandler();

	m_physicsComponent->GetActor().ClearForce( physics::PhysicsDynamicActor::ForceMode::Impulse );
	m_physicsComponent->GetActor().ClearTorque( physics::PhysicsDynamicActor::ForceMode::Impulse );

	m_physicsComponent->GetActor().SetVelocity( Vector3::ZEROS() );
	m_physicsComponent->GetActor().SetAngularVelocity( Vector3::ZEROS() );

	Vector3 delta;

	if ( inputHandler.GetKey( forge::IInput::Key::D ) )
	{
		delta.X = 1.0f;
	}

	if ( inputHandler.GetKey( forge::IInput::Key::A ) )
	{
		delta.X = -1.0f;
	}

	if ( inputHandler.GetKey( forge::IInput::Key::W ) )
	{
		delta.Y = 1.0f;
	}

	if ( inputHandler.GetKey( forge::IInput::Key::S ) )
	{
		delta.Y = -1.0f;
	}

	if ( inputHandler.GetKey( forge::IInput::Key::Q ) )
	{
		delta.Z = -1.0f;
	}

	if ( inputHandler.GetKey( forge::IInput::Key::E ) )
	{
		delta.Z = 1.0f;
	}

	{
		const Float acceleration = 0.003f;
		m_currentSpeed += static_cast< Float >( inputHandler.GetMouseScrollDelta() ) * acceleration;
		m_currentSpeed = Math::Max( 0.1f, m_currentSpeed );

		delta *= Math::Pow( 2.0f, m_currentSpeed );

		if ( inputHandler.GetKey( forge::IInput::Key::Shift ) )
		{
			delta *= 4.0f;
		}

		delta = m_ownerTransform->GetData()->m_transform.GetOrientation().Transform( delta );
		m_physicsComponent->GetActor().AddForce( delta, physics::PhysicsDynamicActor::ForceMode::VelocityChange );
	}

	{
		const Float rotationSpeed = 0.001f;
		auto mouseDelta = inputHandler.GetMousePosDelta();
		Vector3 deltaRot = Vector3{ mouseDelta.X, mouseDelta.Y, 0.0f } * rotationSpeed;

		deltaRot.Z = -deltaRot.X;
		deltaRot.X = deltaRot.Y;
		deltaRot.Y = 0.0f;

		Vector3 eulerAngles = m_ownerTransform->GetWorldOrientation().ToEulerAngles();
		eulerAngles += deltaRot;

		eulerAngles.X = Math::Clamp( -FORGE_PI_HALF, FORGE_PI_HALF, eulerAngles.X );

		m_ownerTransform->SetWorldOrientation( Quaternion( 0.0f, 0.0f, eulerAngles.Z ) * Quaternion( eulerAngles.X, 0.0f, 0.0f ) );
	}

	if ( inputHandler.GetKey( forge::IInput::Key::R ) )
	{
		m_ownerTransform->SetWorldTransform( Transform::IDENTITY() );
		m_ownerTransform->SetWorldOrientation( Quaternion::IDENTITY() );
	}
}
