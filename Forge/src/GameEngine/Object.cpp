#include "Fpch.h"
#include "Object.h"
#include "../ECS/CommandsQueue.h"
#include "../ECS/EntityID.h"
#include "../Core/Serializer.h"

RTTI_IMPLEMENT_TYPE( forge::ObjectFragment );
RTTI_IMPLEMENT_TYPE( forge::Object,
	RTTI_REGISTER_PROPERTY( m_components );
	RTTI_REGISTER_METHOD( Serialize );
	RTTI_REGISTER_METHOD( Deserialize );
);
RTTI_IMPLEMENT_TYPE( forge::ObjectInitData );

forge::Object::Object() = default;
forge::Object::Object( Object&& ) = default;
forge::Object::~Object() = default;

void forge::Object::OnInit( ObjectInitData& initData )
{
	auto& ecsManager = GetEngineInstance().GetECSManager();
	auto& objectsManager = GetEngineInstance().GetObjectsManager();
	ecs::EntityID entityID = objectsManager.GetOrCreateEntityId( GetObjectID() );
	ecsManager.AddFragmentDataToEntity( entityID, forge::ObjectFragment( GetObjectID() ) );
}

void forge::Object::OnDeinit()
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

Bool forge::Object::AddComponent( const forge::IComponent::Type& componentType )
{
	if ( HasComponent( componentType ) )
	{
		return false;
	}

	AttachComponents( { std::unique_ptr< forge::IComponent >( componentType.ConstructTyped() ) }, nullptr );
	return true;
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

		OnComponentDetached( *comp );

		comp->OnDetached( GetEngineInstance(), commandsQueue );
		commandsQueue.Execute();

		m_componentsLUT.erase( it );
		forge::utils::RemoveReorder( m_components, idx );
	}
}

void forge::Object::Serialize( forge::Serializer& serializer )
{
	serializer.Serialize( m_name );

	serializer.Serialize( static_cast< Uint32 >( m_components.size() ) );

	for ( auto& comp : m_components )
	{
		serializer.Serialize( comp->GetType().GetID() );
		serializer.Serialize( comp.get(), comp->GetType() );
	}
}

void forge::Object::Deserialize( forge::Deserializer& deserializer, ObjectInitData& initData )
{
	deserializer.Deserialize( m_name );

	const Uint32 componentsAmount = deserializer.Deserialize< Uint32 >();

	std::vector< std::unique_ptr< IComponent > > deserializedComponents;
	
	for ( Uint32 i = 0; i < componentsAmount; ++i )
	{
		const rtti::ID componentTypeID = deserializer.Deserialize< rtti::ID >();
		if ( const forge::IComponent::Type* type = static_cast< const forge::IComponent::Type* >( rtti::Get().FindType( componentTypeID ) ) )
		{
			FORGE_ASSERT( type->InheritsFromOrIsA< forge::IComponent >() );

			FORGE_ASSERT ( !HasComponent( *type ) );
			deserializedComponents.emplace_back( type->ConstructTyped() );

			if ( const auto* deserializeMethod = type->FindMethod( "Deserialize" ) )
			{
				if ( deserializeMethod->GetReturnTypeDesc() == nullptr && deserializeMethod->GetParametersAmount() == 2
					&& deserializeMethod->GetParameterTypeDesc( 0 )->GetType() == forge::Deserializer::GetTypeStatic()
					&& deserializeMethod->GetParameterTypeDesc( 1 )->GetType() == forge::ObjectInitData::GetTypeStatic() )
				{
					struct Params
					{
						forge::Deserializer deserializer;
						forge::ObjectInitData initData;
					} params{ std::move( deserializer ), std::move( initData ) };
					deserializeMethod->Call( deserializedComponents.back().get(), &params, nullptr );

					deserializer = std::move( params.deserializer );
					initData = std::move( params.initData );
					continue;
				}
			}

			deserializer.Deserialize( deserializedComponents.back().get(), *type );
		}
		else
		{
			FORGE_LOG_ERROR( __FUNCTION__ " Can't find component's type while deserializing");
		}
	}

	AttachComponents( deserializedComponents, &initData );
}

void forge::Object::AttachComponents( forge::ArraySpan< std::unique_ptr< IComponent > > components, forge::ObjectInitData* initData )
{
	std::vector< IComponent* > attachedComponents;
	attachedComponents.reserve( components.GetSize() );

	ecs::CommandsQueue queue( GetEngineInstance().GetECSManager() );

	for ( auto& comp : components )
	{
		comp->Attach( *m_engineInstance, *this, queue, initData );
		attachedComponents.emplace_back( comp.get() );
		FORGE_ASSERT( !m_componentsLUT.contains( &comp->GetType() ) );
		m_componentsLUT[ &comp->GetType() ] = static_cast< Uint32 >( m_components.size() ); 
		m_components.emplace_back( std::move( comp ) );
	}

	queue.Execute();

	for ( IComponent* comp : attachedComponents )
	{
		comp->OnAttached( *m_engineInstance, queue, initData );
		OnComponentAttached( *comp );
	}

	queue.Execute();
}
