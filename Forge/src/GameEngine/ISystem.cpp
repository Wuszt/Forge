#include "Fpch.h"
#include "ISystem.h"

IMPLEMENT_TYPE( systems::ISystem )

systems::ISystem::~ISystem() = default;

void systems::ISystem::Initialize( forge::EngineInstance& engineInstance )
{
	m_engineInstance = &engineInstance;
	OnInitialize();
}

void systems::ISystem::Deinitialize()
{
	OnDeinitialize();
}
