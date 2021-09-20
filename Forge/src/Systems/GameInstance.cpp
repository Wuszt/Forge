#include "Fpch.h"
#include "GameInstance.h"

forge::GameInstance::GameInstance()
{
	m_systemManager = std::make_unique< systems::SystemsManager >( *this );
	m_entitiesManager = std::make_unique< forge::EntitiesManager >( *this );

	m_systemManager->Initialize();
	m_entitiesManager->Initialize();
}
