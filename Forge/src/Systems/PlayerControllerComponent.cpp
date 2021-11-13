#include "Fpch.h"
#include "PlayerControllerComponent.h"
#include "../Core/IWindow.h"
#include "../Core/IInput.h"
#include "TransformComponent.h"

void forge::FreeCameraControllerComponent::OnAttach( EngineInstance& engineInstance )
{
	m_input = engineInstance.GetWindow().GetInput();
	m_ownerTransform = GetOwner().GetComponent< forge::TransformComponent >();
}

void forge::FreeCameraControllerComponent::Update()
{
	Vector3 delta;

	if( m_input->GetKey( forge::IInput::Key::D ) )
	{
		delta.X = 1.0f;
	}

	if( m_input->GetKey( forge::IInput::Key::A ) )
	{
		delta.X = -1.0f;
	}

	if( m_input->GetKey( forge::IInput::Key::W ) )
	{
		delta.Y = 1.0f;
	}

	if( m_input->GetKey( forge::IInput::Key::S ) )
	{
		delta.Y = -1.0f;
	}

	if( m_input->GetKey( forge::IInput::Key::Q ) )
	{
		delta.Z = -1.0f;
	}

	if( m_input->GetKey( forge::IInput::Key::E ) )
	{
		delta.Z = 1.0f;
	}

	{
		const Float acceleration = 0.003f;
		m_currentSpeed += static_cast<Float>( m_input->GetMouseDeltaAxises().Z ) * acceleration;
		m_currentSpeed = Math::Max( 0.1f, m_currentSpeed );

		Vector3 pos = m_ownerTransform->GetData().m_transform.GetPosition3();
		delta *= Math::Pow( 2.0f, m_currentSpeed );

		if( m_input->GetKey( forge::IInput::Key::Shift ) )
		{
			delta *= 4.0f;
		}

		delta = m_ownerTransform->GetData().m_transform.GetOrientation().Transform( delta );
		pos += delta * forge::Time::GetDeltaTime();
		m_ownerTransform->GetData().m_transform.SetPosition( pos );
	}

	{
		const Float rotationSpeed = 0.001f;
		Vector3 deltaRot = m_input->GetMouseDeltaAxises() * rotationSpeed;

		deltaRot.Z = -deltaRot.X;
		deltaRot.X = deltaRot.Y;
		deltaRot.Y = 0.0f;

		m_eulerAngles += deltaRot;

		m_eulerAngles.X = Math::Clamp( -FORGE_PI_HALF, FORGE_PI_HALF, m_eulerAngles.X );

		m_ownerTransform->GetData().m_transform.SetOrientation( Quaternion( 0.0f, 0.0f, m_eulerAngles.Z ) * Quaternion( m_eulerAngles.X, 0.0f, 0.0f ) );
	}

	if( m_input->GetKey( forge::IInput::Key::R ) )
	{
		m_ownerTransform->GetData().m_transform = Transform::IDENTITY();
		m_eulerAngles = Vector3::ZEROS();
	}
}
