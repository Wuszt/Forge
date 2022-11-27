#pragma once

namespace forge
{
	class IComponent
	{
		DECLARE_ABSTRACT_TYPE( IComponent );
	public:
		template< class T >
		friend T* Entity::AddComponent<T>();
		friend void Entity::OnDetach();

		IComponent();
		virtual ~IComponent();

		Entity& GetOwner() const
		{
			return *m_owner;
		}

	protected:
		virtual void OnAttach( EngineInstance& engineInstance ) {}
		virtual void OnDetach( EngineInstance& engineInstance ) {}

	private:
		void Attach( EngineInstance& engineInstance, Entity& owner );
		void Detach( EngineInstance& engineInstance );

		Entity* m_owner;
	};

	template< class TData >
	class DataComponent : public IComponent
	{
	public:
		using IComponent::IComponent;

		virtual void OnAttach( EngineInstance& engineInstance ) override
		{
			auto& systemsManager = engineInstance.GetSystemsManager();

			systemsManager.AddECSData< TData >( GetOwner().GetEntityID() );
		}

		virtual void OnDetach( EngineInstance& engineInstance ) override
		{
			auto& systemsManager = engineInstance.GetSystemsManager();

			systemsManager.RemoveECSData< TData >( GetOwner().GetEntityID() );
		}

		TData& GetData()
		{
			auto& ei = GetOwner().GetEngineInstance();
			systems::SystemsManager& sm = ei.GetSystemsManager();
			const auto& archetypes = sm.GetArchetypesOfEntity( GetOwner().GetEntityID() );

			auto found = std::find_if( archetypes.begin(), archetypes.end(), []( auto* archetype ) { return archetype->ContainsData< TData >(); } );
			if( found == archetypes.end() )
			{
				found = found;
			}
			FORGE_ASSERT( found != archetypes.end() );
			return ( *found )->GetData< TData >( GetOwner().GetEntityID() );
		}

	private:
	};
}