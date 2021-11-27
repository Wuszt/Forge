#pragma once

namespace forge
{
	class IComponent
	{
	public:
		IComponent( Entity& owner );
		virtual ~IComponent() {}

		virtual void OnAttach( EngineInstance& engineInstance ) {}
		virtual void OnDetach( EngineInstance& engineInstance ) {}

		Entity& GetOwner() const
		{
			return m_owner;
		}

	private:
		Entity& m_owner;
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
			FORGE_ASSERT( found != archetypes.end() );			
			return (*found)->GetData< TData >( GetOwner().GetEntityID() );
		}

	private:
	};
}

