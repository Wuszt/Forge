#include "Fpch.h"
#include "SystemsManager.h"
#include "ISystem.h"

systems::SystemsManager::SystemsManager( forge::EngineInstance& engineInstance )
	: m_engineInstance( engineInstance )
{}

systems::SystemsManager::~SystemsManager()
{
	for ( auto& sys : m_systems )
	{
		sys->Deinitialize();
	}
}

void systems::SystemsManager::AddSystems( forge::ArraySpan< const ISystem::Type* > systemsClasses )
{
	Uint32 prevCount = static_cast< Uint32 >( m_systems.size() );
	for ( const auto* type : systemsClasses )
	{
		auto* system = m_systems.emplace_back( type->ConstructTyped() ).get();
		m_systemsLUT.emplace( type, system );
	}

	for ( Uint32 i = prevCount; i < static_cast< Uint32 >( m_systems.size() ); ++i )
	{
		m_systems[ i ]->Initialize( m_engineInstance );
	}

	for ( Uint32 i = prevCount; i < static_cast< Uint32 >( m_systems.size() ); ++i )
	{
		m_systems[ i ]->PostInit();
	}
}
