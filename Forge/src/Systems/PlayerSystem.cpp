#include "Fpch.h"
#include "PlayerSystem.h"
#include "PlayerControllerComponent.h"
#include "../Core/IInput.h"
#include "../Core/IWindow.h"

void systems::PlayerSystem::OnInitialize()
{
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update, std::bind( &systems::PlayerSystem::Update, this ) );
}

forge::Entity* systems::PlayerSystem::GetCurrentPlayerEntity() const
{
	return m_activeController ? &m_activeController->GetOwner() : nullptr;
}

void systems::PlayerSystem::Update()
{
	auto input = GetEngineInstance().GetWindow().GetInput();
	if( input->GetKey( forge::IInput::Key::Shift ) && input->GetMouseButton( forge::IInput::MouseButton::MiddleButton ) )
	{
		if( !m_wasShiftAndWheelPressed )
		{
			input->LockCursor( !input->IsCursorLocked() );
		}

		m_wasShiftAndWheelPressed = true;
	}
	else
	{
		m_wasShiftAndWheelPressed = false;
	}

	if( input->IsCursorLocked() )
	{
		if( m_activeController )
		{
			m_activeController->Update();
		}
	}
}
