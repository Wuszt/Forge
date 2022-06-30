#include "Fpch.h"
#include "SystemsManager.h"
#include "ISystem.h"

void systems::SystemsManager::Initialize()
{
	m_onEntityCreated = m_engineInstance.GetEntitiesManager().GetEntityCreatedCallback().AddListener(
		[ this ]( forge::EntityID id )
	{
		for( auto& archetype : m_archetypes )
		{
			archetype->OnEntityCreated();
		}
	} );

	m_onEntityDestructed = m_engineInstance.GetEntitiesManager().GetEntityDestructedCallback().AddListener(
		[ this ]( forge::EntityID id )
	{
		for( auto& archetype : m_archetypes )
		{
			archetype->OnEntityDestructed( id );
		}
	} );

	m_onTick = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::PostRendering, [ this ]()
	{
		for( auto& archetype : m_archetypes )
		{
			archetype->SetDirty( false );
		}
	} );
}

void systems::SystemsManager::Deinitialize()
{
	m_onEntityCreated.Unregister();
	m_onTick.Unregister();
}

Uint32 GetTypesHash( const forge::ArraySpan< const std::unique_ptr< forge::IDataPackage > >& dataPackages )
{
	Uint32 typesHash = 0u;

	for( auto& package : dataPackages )
	{
		typesHash ^= static_cast<Uint32>( package->GetType().GetHash() );
	}

	return typesHash;
}

forge::ArraySpan< systems::Archetype* > systems::SystemsManager::GetArchetypesWithDataTypes( const systems::IArchetypeDataTypes& archetypeDataTypes )
{
	return m_archetypeTypesHashToArchetypesLUT[ GetTypesHash( archetypeDataTypes.GatherDataPackages() ) ];
}

void systems::SystemsManager::AddECSData( forge::EntityID id, std::unique_ptr< forge::IDataPackage > package )
{
	PC_SCOPE_FUNC();

	const rtti::IType& type = package->GetType();
	FORGE_ASSERT( std::count( m_entityDataTypesLUT[ id ].begin(), m_entityDataTypesLUT[ id ].end(), &type ) == 0u );
	m_entityDataTypesLUT[ id ].emplace_back( &type );

	auto& entityTypes = m_entityDataTypesLUT.at( id );

	std::vector< std::unique_ptr< forge::IDataPackage > > newArchetypePackages;
	newArchetypePackages.emplace_back( std::move( package ) );

	for( auto& system : m_ecsSystems )
	{
		auto archetypesDataTypes = system->GetArchetypesDataTypes();

		for( auto& archetypeDataTypes : archetypesDataTypes )
		{
			auto requiredTypes = archetypeDataTypes->GetRequiredDataTypes();

			if( std::find( requiredTypes.begin(), requiredTypes.end(), &type ) != requiredTypes.end() )
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
					auto requiredDataPackages = archetypeDataTypes->GatherDataPackages();

					for( auto& requiredDataPackage : requiredDataPackages )
					{
						auto found = std::find_if( newArchetypePackages.begin(), newArchetypePackages.end(), [ &requiredDataPackage ]( const auto& package ) { return requiredDataPackage->GetType() == package->GetType(); } );
						if( found == newArchetypePackages.end() )
						{
							newArchetypePackages.emplace_back( std::move( requiredDataPackage ) );
						}
					}
				}
			}
		}
	}

	std::vector< Archetype* > donors;
	Uint32 typesHash = GetTypesHash( newArchetypePackages );
	std::vector< const rtti::IType* > types;
	for( auto& package : newArchetypePackages )
	{
		types.emplace_back( &package->GetType() );
		auto& entityArchetypes = m_entityArchetypesLUT[ id ];
		for( auto it = entityArchetypes.begin(); it != entityArchetypes.end(); )
		{
			if( ( *it )->ContainsData( package->GetType() ) )
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

		for( auto& package : newArchetypePackages )
		{
			m_dataToArchetypesLUT[ &package->GetType() ].emplace_back( archetype );
			archetype->AddDataPackage( std::move( package ) );
		}
	}
	else
	{
		archetype = found->second;
		archetype->GetData( type ).AddEmptyData();
	}

	for( auto& system : m_ecsSystems )
	{
		auto archetypesDataTypes = system->GetArchetypesDataTypes();

		for( auto& archetypeDataTypes : archetypesDataTypes )
		{
			Bool hasArchetypeAllRequiredTypes = true;
			for( auto requiredDataType : archetypeDataTypes->GetRequiredDataTypes() )
			{
				if( std::find_if( types.begin(), types.end(), [ &requiredDataType ]( const rtti::IType* type ) { return type == requiredDataType; } ) == types.end() )
				{
					hasArchetypeAllRequiredTypes = false;
					break;
				}
			}

			Uint32 typesHash = GetTypesHash( archetypeDataTypes->GatherDataPackages() );

			if( hasArchetypeAllRequiredTypes )
			{
				if( std::find( m_archetypeTypesHashToArchetypesLUT[ typesHash ].begin(), m_archetypeTypesHashToArchetypesLUT[ typesHash ].end(), archetype ) == m_archetypeTypesHashToArchetypesLUT[ typesHash ].end() )
				{
					m_archetypeTypesHashToArchetypesLUT[ typesHash ].emplace_back( archetype );
				}
			}
		}
	}

	archetype->MoveEntityFrom( id, donors );

	m_entityArchetypesLUT[ id ].emplace_back( archetype );
	archetype->SetDirty( true );
}

void systems::SystemsManager::RemoveECSData( forge::EntityID id, const rtti::IType& type )
{
	PC_SCOPE_FUNC();

	auto& entityTypes = m_entityDataTypesLUT.at( id );
	auto& entityArchetypes = m_entityArchetypesLUT.at( id );

	auto currentArchetypeIt = std::find_if( entityArchetypes.begin(), entityArchetypes.end(), [ & ]( systems::Archetype* archetype ) { return archetype->ContainsData( type ); } );

	FORGE_ASSERT( currentArchetypeIt != entityArchetypes.end() );

	std::vector< std::unique_ptr< forge::IDataPackage > > tmpDataPackages;
	( *currentArchetypeIt )->MoveEntityTo( id, tmpDataPackages );

	( *currentArchetypeIt )->SetDirty( true );

	if( ( *currentArchetypeIt )->IsEmpty() )
	{
		m_typesHashToArchetypeLUT.erase( GetTypesHash( tmpDataPackages ) );

		for( const auto& dataPackage : tmpDataPackages )
		{
			forge::utils::RemoveValueReorder( m_dataToArchetypesLUT.at( &dataPackage->GetType() ), *currentArchetypeIt );
		}

		for( auto& it : m_archetypeTypesHashToArchetypesLUT )
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
		forge::utils::RemoveValueReorder( entityTypes, &dataPackage->GetType() );
	}

	forge::utils::RemoveReorder( entityArchetypes, currentArchetypeIt );

	for( auto& dataPackage : tmpDataPackages )
	{
		if( dataPackage->GetType() != type )
		{
			AddECSData( id, std::move( dataPackage ) );
		}
	}
}

void systems::SystemsManager::Boot( const BootContext& ctx )
{
	FORGE_ASSERT( m_allSystems.empty(), "SystemsManager is already booted" );

	const auto& systemsClasses = ctx.GetSystemsClasses();
	for( const systems::ISystem::ClassType* systemClass : systemsClasses )
	{
		std::unique_ptr< ISystem > sys = systemClass->CreateDefault();

		ISystem* rawSys = nullptr;

		m_allSystems.emplace_back( std::move( sys ) );
		rawSys = m_allSystems.back().get();

		if( rawSys->IsECSSystem() )
		{
			m_ecsSystems.emplace_back( static_cast< IECSSystem* >( rawSys ) );
		}

		m_systemsLUT.emplace( &rawSys->GetType(), rawSys );
	}

	for( auto& sys : m_allSystems )
	{
		sys->Initialize( GetEngineInstance() );
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
