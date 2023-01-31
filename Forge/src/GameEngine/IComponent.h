#pragma once
#include "../ECS/EntityID.h"
#include "../ECS/ECSManager.h"

namespace forge
{
	class IComponent
	{
		DECLARE_ABSTRACT_CLASS( IComponent );
	public:
		template< class T >
		friend T* Object::AddComponent<T>();
		friend void Object::OnDetach();

		IComponent();
		virtual ~IComponent() = 0;

		Object& GetOwner() const
		{
			return *m_owner;
		}

	protected:
		virtual void OnAttach( EngineInstance& engineInstance ) {}
		virtual void OnDetach( EngineInstance& engineInstance ) {}

	private:
		void Attach( EngineInstance& engineInstance, Object& owner );
		void Detach( EngineInstance& engineInstance );

		Object* m_owner;
	};

	template< class TData >
	class DataComponent : public IComponent
	{
	public:
		using IComponent::IComponent;

		virtual void OnAttach( EngineInstance& engineInstance ) override
		{
			auto& objectsManager = engineInstance.GetObjectsManager();
			auto& ecsManager = engineInstance.GetECSManager();

			ecs::EntityID id = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
			ecsManager.AddFragmentToEntity< TData >( id );
		}

		virtual void OnDetach( EngineInstance& engineInstance ) override
		{
			auto& objectsManager = engineInstance.GetObjectsManager();
			auto& ecsManager = engineInstance.GetECSManager();

			ecs::EntityID id = objectsManager.GetOrCreateEntityId( GetOwner().GetObjectID() );
			ecsManager.RemoveFragmentFromEntity< TData >( id );
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