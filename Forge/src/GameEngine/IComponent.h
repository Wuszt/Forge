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

	namespace datacomponent::internal
	{
		void OnAttaching( forge::Object& owner, const ecs::Fragment::Type& fragmentType, ecs::CommandsQueue& commandsQueue );
		void OnDetaching( forge::Object& owner, const ecs::Fragment::Type& fragmentType, ecs::CommandsQueue& commandsQueue );
		ecs::EntityID GetObjectEntityID( forge::Object& owner );
	}

	template< class TData, class ParentClass = IComponent >
	class DataComponent : public ParentClass
	{
	public:
		using ParentClass::ParentClass;
		using ParentClass::GetOwner;

		virtual void OnAttaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override
		{
			ParentClass::OnAttaching( engineInstance, commandsQueue );
			datacomponent::internal::OnAttaching( GetOwner(), TData::GetTypeStatic(), commandsQueue );
		}

		virtual void OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override
		{
			ParentClass::OnDetaching( engineInstance, commandsQueue );
			datacomponent::internal::OnDetaching( GetOwner(), TData::GetTypeStatic(), commandsQueue );
		}

		ecs::FragmentView< TData > GetData() const
		{
			auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
			ecs::EntityID id = datacomponent::internal::GetObjectEntityID( GetOwner() );
			return ecsManager.GetFragmentView< TData >( id );
		}

	protected:
		ecs::MutableFragmentView< TData > GetMutableData()
		{
			auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
			ecs::EntityID id = datacomponent::internal::GetObjectEntityID( GetOwner() );
			return ecsManager.GetMutableFragmentView< TData >( id );
		}

	private:
	};
}