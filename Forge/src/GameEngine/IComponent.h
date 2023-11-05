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
		RTTI_DECLARE_ABSTRACT_CLASS( IComponent );
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

	template< class TData, class ParentClass = IComponent >
	class DataComponent : public ParentClass
	{
	public:
		using ParentClass::ParentClass;
		using ParentClass::GetOwner;

		virtual void OnAttaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override
		{
			ParentClass::OnAttaching( engineInstance, commandsQueue );
			auto& objectsManager = engineInstance.GetObjectsManager();
			auto& ecsManager = engineInstance.GetECSManager();

			ecs::EntityID id = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
			commandsQueue.AddFragment( id, TData::GetTypeStatic() );
		}

		virtual void OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override
		{
			ParentClass::OnDetaching( engineInstance, commandsQueue );
			auto& objectsManager = engineInstance.GetObjectsManager();
			auto& ecsManager = engineInstance.GetECSManager();

			ecs::EntityID id = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
			commandsQueue.RemoveFragment( id, TData::GetTypeStatic() );
		}

		const TData& GetData() const
		{
			auto& objectsManager = GetOwner().GetEngineInstance().GetObjectsManager();
			auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();

			ecs::EntityID id = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
			return *ecsManager.GetFragment< TData >( id );
		}

	protected:
		TData& GetMutableData()
		{
			auto& objectsManager = GetOwner().GetEngineInstance().GetObjectsManager();
			auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();

			ecs::EntityID id = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
			return *ecsManager.GetMutableFragment< TData >( id );
		}

	private:
	};
}