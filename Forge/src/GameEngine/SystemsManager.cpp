#include "Fpch.h"
#include "SystemsManager.h"
#include "ISystem.h"

systems::SystemsManager::SystemsManager( forge::EngineInstance& engineInstance )
	: m_engineInstance( engineInstance )
{}

void systems::SystemsManager::Boot( const BootContext& ctx )
{
	FORGE_ASSERT( m_allSystems.empty() == true, "SystemsManager is already booted" );

	const auto& systemsClasses = ctx.GetSystemsClasses();
	for( const systems::ISystem::ClassType* systemClass : systemsClasses )
	{
		std::unique_ptr< ISystem > sys = systemClass->CreateDefault();

		ISystem* rawSys = nullptr;

		m_allSystems.emplace_back( std::move( sys ) );
		rawSys = m_allSystems.back().get();

		m_systemsLUT.emplace( &rawSys->GetType(), rawSys );
	}

	for( auto& sys : m_allSystems )
	{
		sys->Initialize( m_engineInstance );
	}

	m_onBootCallback.Invoke();
}

void systems::SystemsManager::Shutdown()
{
	for( auto& sys : m_allSystems )
	{
		sys->Deinitialize();
	}
}
