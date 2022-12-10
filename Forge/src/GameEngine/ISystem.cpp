#include "Fpch.h"
#include "ISystem.h"

IMPLEMENT_TYPE( systems::ISystem )

void systems::ISystem::Initialize( forge::EngineInstance& engineInstance )
{
	m_engineInstance = &engineInstance;
	OnInitialize();
}

void systems::ISystem::Deinitialize()
{
	OnDeinitialize();
}
