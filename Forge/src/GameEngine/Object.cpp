#include "Fpch.h"
#include "Object.h"
#include "../ECS/CommandsQueue.h"
#include "../ECS/EntityID.h"

RTTI_IMPLEMENT_TYPE( forge::ObjectFragment );

forge::Object::Object( EngineInstance& engineInstance, ObjectID id )
	: m_engineInstance( engineInstance )
	, m_id( id )
{}

forge::Object::~Object() = default;

void forge::Object::OnAttach()
{
	auto& ecsManager = GetEngineInstance().GetECSManager();
	auto& objectsManager = GetEngineInstance().GetObjectsManager();
	ecs::EntityID entityID = objectsManager.GetOrCreateEntityId( GetObjectID() );
	ecsManager.AddFragmentToEntity( entityID, forge::ObjectFragment::GetTypeStatic() );
	ecsManager.GetFragment< forge::ObjectFragment >( entityID )->m_objectID = GetObjectID();
}

void forge::Object::OnDetach()
{
	ecs::CommandsQueue commandsQueue;

	if ( GetEngineInstance().GetObjectsManager().HasEntity( GetObjectID() ) )
	{
		commandsQueue.RemoveEntity( GetEngineInstance().GetObjectsManager().GetOrCreateEntityId( GetObjectID() ) );
	}

	for( auto& comp : m_components )
	{
		comp.second->Detach( m_engineInstance, commandsQueue );
	}

	commandsQueue.Execute( GetEngineInstance().GetECSManager() );

	for ( auto& comp : m_components )
	{
		comp.second->OnDetached( m_engineInstance, commandsQueue );
	}

	commandsQueue.Execute( GetEngineInstance().GetECSManager() );
}

void forge::Object::AttachComponents( std::vector< std::unique_ptr< IComponent > >&& components )
{
	std::vector< IComponent* > attachedComponents;
	attachedComponents.reserve( components.size() );

	ecs::CommandsQueue queue;

	for ( auto& comp : components )
	{
		comp->Attach( m_engineInstance, *this, queue );
		attachedComponents.emplace_back( comp.get() );
		m_components.emplace( &comp->GetType(), std::move( comp ) );
	}

	queue.Execute( GetEngineInstance().GetECSManager() );

	for ( IComponent* comp : attachedComponents )
	{
		comp->OnAttached( m_engineInstance, queue );
	}

	queue.Execute( GetEngineInstance().GetECSManager() );
}

void forge::Object::RequestAddingComponentsInternal( const std::function< void() >& creationFunc )
{
	GetEngineInstance().GetObjectsManager().RequestAddingComponentsToObject( creationFunc );
}
