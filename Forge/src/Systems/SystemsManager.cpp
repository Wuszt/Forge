#include "Fpch.h"
#include "SystemsManager.h"
#include "ISystem.h"

void systems::SystemsManager::Initialize()
{
	m_onEntityCreated = std::make_unique< forge::CallbackToken >( m_engineInstance.GetEntitiesManager().GetEntityCreatedCallback().AddListener(
		[ this ]( forge::EntityID id )
	{
		for( auto& archetype : m_archetypes )
		{
			archetype->OnEntityCreated();
		}
	} ) );

	m_onEntityDestructed = std::make_unique< forge::CallbackToken >( m_engineInstance.GetEntitiesManager().GetEntityDestructedCallback().AddListener(
		[ this ]( forge::EntityID id )
	{
		for( auto& archetype : m_archetypes )
		{
			archetype->OnEntityDestructed( id );
		}
	} ) );

	m_onTick = std::make_unique< forge::CallbackToken >( GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostRendering, [ this ]()
	{
		for( auto& archetype : m_archetypes )
		{
			archetype->SetDirty( false );
		}
	} ) );
}

void systems::SystemsManager::Deinitialize()
{
	m_onEntityCreated = nullptr;
	m_onTick = nullptr;
}

Uint32 GetTypesHash( const std::vector< std::unique_ptr< forge::IDataPackage > >& dataPackages )
{
	Uint32 typesHash = 0u;

	for( auto& package : dataPackages )
	{
		typesHash ^= static_cast<Uint32>( package->GetTypeIndex().hash_code() );
	}

	return typesHash;
}

void systems::SystemsManager::AddECSData( forge::EntityID id, std::unique_ptr< forge::IDataPackage > package )
{
	PC_SCOPE_FUNC();

	auto typeIndex = package->GetTypeIndex();
	FORGE_ASSERT( std::count( m_entityDataTypesLUT[ id ].begin(), m_entityDataTypesLUT[ id ].end(), typeIndex ) == 0u );
	m_entityDataTypesLUT[ id ].emplace_back( typeIndex );

	auto& entityTypes = m_entityDataTypesLUT.at( id );

	std::vector< std::unique_ptr< forge::IDataPackage > > newArchetypePackages;
	newArchetypePackages.emplace_back( std::move( package ) );

	for( auto& system : m_ecsSystems )
	{
		auto requiredTypes = system->GetRequiredDataTypes();
		if( std::find( requiredTypes.begin(), requiredTypes.end(), typeIndex ) != requiredTypes.end() )
		{
			Bool entityHasAllTypes = true;
			for( auto requiredType : requiredTypes )
			{
				if( std::find( entityTypes.begin(), entityTypes.end(), requiredType ) == entityTypes.end() )
				{
					entityHasAllTypes = false;
					break;
				}
			}

			if( entityHasAllTypes )
			{
				auto requiredDataPackages = system->CreateRequiredDataPackages();

				for( auto& requiredDataPackage : requiredDataPackages )
				{
					auto found = std::find_if( newArchetypePackages.begin(), newArchetypePackages.end(), [ &requiredDataPackage ]( const auto& package ) { return requiredDataPackage->GetTypeIndex() == package->GetTypeIndex(); } );
					if( found == newArchetypePackages.end() )
					{
						newArchetypePackages.emplace_back( std::move( requiredDataPackage ) );
					}
				}
			}
		}
	}

	std::vector< Archetype* > donors;
	Uint32 typesHash = GetTypesHash( newArchetypePackages );
	for( auto& package : newArchetypePackages )
	{
		auto& entityArchetypes = m_entityArchetypesLUT[ id ];
		for( auto it = entityArchetypes.begin(); it != entityArchetypes.end(); )
		{
			if( ( *it )->ContainsData( package->GetTypeIndex() ) )
			{
				( *it )->SetDirty( true );
				donors.emplace_back( *it );
				it = forge::utils::RemoveReorder( entityArchetypes, it );
			}
			else
			{
				++it;
			}
		}
	}

	Archetype* archetype = nullptr;
	auto found = m_typesHashToArchetypeLUT.find( typesHash );
	if( found == m_typesHashToArchetypeLUT.end() )
	{
		m_archetypes.emplace_back( std::make_unique< Archetype >( m_engineInstance.GetEntitiesManager().GetHighestID() ) );
		archetype = m_archetypes.back().get();
		m_typesHashToArchetypeLUT.emplace( typesHash, archetype );

		for( auto& system : m_ecsSystems )
		{
			auto& requiredDataTypes = system->GetRequiredDataTypes();

			Bool hasArchetypeAllRequiredTypes = true;
			for( auto requiredDataType : requiredDataTypes )
			{
				if( std::find_if( newArchetypePackages.begin(), newArchetypePackages.end(), [ &requiredDataType ]( auto& package ) { return package->GetTypeIndex() == requiredDataType; } ) == newArchetypePackages.end() )
				{
					hasArchetypeAllRequiredTypes = false;
					break;
				}
			}

			if( hasArchetypeAllRequiredTypes )
			{
				m_systemToArchetypesLUT[ typeid( *system ) ].emplace_back( archetype );
			}
		}

		for( auto& package : newArchetypePackages )
		{
			m_dataToArchetypesLUT[ package->GetTypeIndex() ].emplace_back( archetype );
			archetype->AddDataPackage( std::move( package ) );
		}
	}
	else
	{
		archetype = found->second;
		archetype->GetData( typeIndex ).AddEmptyData();
	}

	archetype->MoveEntityFrom( id, donors );

	m_entityArchetypesLUT[ id ].emplace_back( archetype );
	archetype->SetDirty( true );
}

void systems::SystemsManager::RemoveECSData( forge::EntityID id, std::type_index typeIndex )
{
	PC_SCOPE_FUNC();

	auto& entityTypes = m_entityDataTypesLUT.at( id );
	auto& entityArchetypes = m_entityArchetypesLUT.at ( id );

	auto currentArchetypeIt = std::find_if( entityArchetypes.begin(), entityArchetypes.end(), [ & ]( systems::Archetype* archetype ) { return archetype->ContainsData( typeIndex ); } );

	FORGE_ASSERT( currentArchetypeIt != entityArchetypes.end() );

	std::vector< std::unique_ptr< forge::IDataPackage > > tmpDataPackages;
	(*currentArchetypeIt)->MoveEntityTo( id, tmpDataPackages );

	(*currentArchetypeIt)->SetDirty( true );

	if( ( *currentArchetypeIt )->IsEmpty() )
	{
		m_typesHashToArchetypeLUT.erase( GetTypesHash( tmpDataPackages ) );

		for( const auto& dataPackage : tmpDataPackages )
		{
			forge::utils::RemoveValueReorder( m_dataToArchetypesLUT.at( dataPackage->GetTypeIndex() ), *currentArchetypeIt );
		}

		for( auto& it : m_systemToArchetypesLUT )
		{
			auto found = std::find( it.second.begin(), it.second.end(), ( *currentArchetypeIt ) );

			if( found != it.second.end() )
			{
				forge::utils::RemoveReorder( it.second, found );
			}
		}

		auto archetypeToRemoveIt = std::find_if( m_archetypes.begin(), m_archetypes.end(), [ & ]( const auto& archetype ) { return *currentArchetypeIt == archetype.get(); } );
		FORGE_ASSERT( archetypeToRemoveIt != m_archetypes.end() );
		forge::utils::RemoveReorder( m_archetypes, archetypeToRemoveIt );
	}

	for( const auto& dataPackage : tmpDataPackages )
	{
		forge::utils::RemoveValueReorder( entityTypes, dataPackage->GetTypeIndex() );
	}

	forge::utils::RemoveReorder( entityArchetypes, currentArchetypeIt );

	for( auto& dataPackage : tmpDataPackages )
	{
		if( dataPackage->GetTypeIndex() != typeIndex )
		{
			AddECSData( id, std::move( dataPackage ) );
		}
	}
}

void systems::SystemsManager::Boot( const BootContext& ctx )
{
	FORGE_ASSERT( m_systems.empty(), "SystemsManager is already booted" );

	const auto& systemsCreations = ctx.GetSystemsCreations();
	for( const auto& systemCreation : systemsCreations )
	{
		std::unique_ptr< ISystem > sys = systemCreation( GetEngineInstance() );

		ISystem* rawSys = nullptr;

		if( dynamic_cast<IECSSystem*>( sys.get() ) )
		{
			m_ecsSystems.emplace_back( static_cast< IECSSystem* >( sys.release() ) );
			rawSys = m_ecsSystems.back().get();
		}
		else
		{
			m_systems.emplace_back( std::move( sys ) );
			rawSys = m_systems.back().get();
		}

		m_systemsLUT.emplace( std::type_index( typeid( *rawSys ) ), rawSys );
	}

	for( auto& sys : m_systems )
	{
		sys->OnInitialize();
	}

	for( auto& sys : m_ecsSystems )
	{
		sys->OnInitialize();
	}

	m_onBootCallback.Invoke();
}

void systems::SystemsManager::Shutdown()
{
	for( auto& sys : m_systems )
	{
		sys->OnDeinitialize();
	}

	for( auto& sys : m_ecsSystems )
	{
		sys->OnDeinitialize();
	}
}
