#include "Fpch.h"
#include "Object.h"
#include "../ECS/CommandsQueue.h"
#include "../ECS/EntityID.h"

RTTI_IMPLEMENT_TYPE( forge::ObjectFragment );
RTTI_IMPLEMENT_TYPE( forge::Object,
	RTTI_REGISTER_PROPERTY( m_components )
);

forge::Object::Object() = default;
forge::Object::Object( Object&& ) = default;
forge::Object::~Object() = default;

void forge::Object::OnAttach()
{
	auto& ecsManager = GetEngineInstance().GetECSManager();
	auto& objectsManager = GetEngineInstance().GetObjectsManager();
	ecs::EntityID entityID = objectsManager.GetOrCreateEntityId( GetObjectID() );
	ecsManager.AddFragmentDataToEntity( entityID, forge::ObjectFragment( GetObjectID() ) );
}

void forge::Object::OnDetach()
{
	ecs::CommandsQueue commandsQueue( GetEngineInstance().GetECSManager() );

	if ( GetEngineInstance().GetObjectsManager().HasEntity( GetObjectID() ) )
	{
		commandsQueue.RemoveEntity( GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetObjectID() ) );
	}

	for( auto& comp : m_components )
	{
		comp->Detach( *m_engineInstance, commandsQueue );
	}

	commandsQueue.Execute();

	for ( auto& comp : m_components )
	{
		comp->OnDetached( *m_engineInstance, commandsQueue );
	}

	commandsQueue.Execute();

	m_componentsLUT.clear();
	m_components.clear();
}

void forge::Object::AddComponent( const forge::IComponent::Type& componentType )
{
	AttachComponents( { componentType.Construct() } );
}

void forge::Object::RemoveComponent( const forge::IComponent::Type& componentType )
{
	auto it = m_componentsLUT.find( &componentType );
	if ( it != m_componentsLUT.end() )
	{
		Uint32 idx = it->second;
		m_componentsLUT[ &m_components[ m_components.size() - 1 ]->GetType() ] = idx;

		auto& comp = m_components[ idx ];
		ecs::CommandsQueue commandsQueue( GetEngineInstance().GetECSManager() );
		
		comp->Detach( GetEngineInstance(), commandsQueue );
		commandsQueue.Execute();

		comp->OnDetached( GetEngineInstance(), commandsQueue );
		commandsQueue.Execute();

		m_componentsLUT.erase( it );
		forge::utils::RemoveReorder( m_components, idx );
	}
}

void forge::Object::AttachComponents( forge::ArraySpan< std::unique_ptr< IComponent > > components )
{
	std::vector< IComponent* > attachedComponents;
	attachedComponents.reserve( components.GetSize() );

	ecs::CommandsQueue queue( GetEngineInstance().GetECSManager() );

	for ( auto& comp : components )
	{
		comp->Attach( *m_engineInstance, *this, queue );
		attachedComponents.emplace_back( comp.get() );
		FORGE_ASSERT( !m_componentsLUT.contains( &comp->GetType() ) );
		m_componentsLUT[ &comp->GetType() ] = static_cast< Uint32 >( m_components.size() ); 
		m_components.emplace_back( std::move( comp ) );
	}

	queue.Execute();

	for ( IComponent* comp : attachedComponents )
	{
		comp->OnAttached( *m_engineInstance, queue );
	}

	queue.Execute();
}
