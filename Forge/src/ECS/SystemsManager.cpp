#include "Fpch.h"
#include "SystemsManager.h"
#include "ISystem.h"

void ecs::SystemsManager::Boot( const BootContext& ctx )
{
	const auto& systemsCreations = ctx.GetSystemsCreations();

	for( const auto& systemCreation : systemsCreations )
	{
		std::unique_ptr< ISystem > sys = systemCreation();

		m_systemsLUT.emplace( std::type_index( typeid( *sys ) ), static_cast< Uint32 >( m_systems.size() ) );
		m_systems.emplace_back( std::move( sys ) );
	}

	for( auto& sys : m_systems )
	{
		sys->OnAttach();
	}
}

void ecs::SystemsManager::Update()
{
	for( auto& sys : m_systems )
	{
		sys->Update();
	}
}

void ecs::SystemsManager::Shutdown()
{
	for( auto& sys : m_systems )
	{
		sys->OnDetach();
	}
}
