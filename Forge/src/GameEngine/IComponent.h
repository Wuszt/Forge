#pragma once
#include "../ECS/EntityID.h"
#include "../ECS/ECSManager.h"
#include "../ECS/CommandsQueue.h"

namespace ecs
{
	class CommandsQueue;
}

namespace forge
{
	class IComponent
	{
		DECLARE_ABSTRACT_CLASS( IComponent );
	public:
		friend class Object;

		IComponent();
		virtual ~IComponent() = 0;

		Object& GetOwner() const
		{
			return *m_owner;
		}

	protected:
		virtual void OnAttaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) {}
		virtual void OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) {}

		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) {}
		virtual void OnDetached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) {}

	private:
		void Attach( EngineInstance& engineInstance, Object& owner, ecs::CommandsQueue& commandsQueue );
		void Detach( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue );

		Object* m_owner;
	};

	template< class TData >
	class DataComponent : public IComponent
	{
	public:
		using IComponent::IComponent;

		virtual void OnAttaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override
		{
			IComponent::OnAttaching( engineInstance, commandsQueue );
			auto& objectsManager = engineInstance.GetObjectsManager();
			auto& ecsManager = engineInstance.GetECSManager();

			ecs::EntityID id = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
			commandsQueue.AddFragment( id, TData::GetTypeStatic() );
		}

		virtual void OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override
		{
			IComponent::OnDetaching( engineInstance, commandsQueue );
			auto& objectsManager = engineInstance.GetObjectsManager();
			auto& ecsManager = engineInstance.GetECSManager();

			ecs::EntityID id = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
			commandsQueue.RemoveFragment( id, TData::GetTypeStatic() );
		}

		const TData& GetData() const
		{
			return const_cast< DataComponent< TData >* >( this )->GetMutableData();
		}

	protected:
		TData& GetMutableData()
		{
			auto& objectsManager = GetOwner().GetEngineInstance().GetObjectsManager();
			auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();

			ecs::EntityID id = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
			auto* archetype = ecsManager.GetEntityArchetype( id );
			return *archetype->GetFragment< TData >( id );
		}

	private:
	};
}