#pragma once

namespace forge
{
	class IComponent
	{
	public:
		IComponent( Entity& owner );
		virtual ~IComponent() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

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
		
		virtual void OnAttach() override
		{
			auto& gi = GetOwner().GetEntitiesManager().GetGameInstance();
			auto& systemsManager = gi.GetSystemsManager();

			systemsManager.AddECSData< TData >( GetOwner().GetEntityID() );
		}

		TData& GetData()
		{
			auto& gi = GetOwner().GetEntitiesManager().GetGameInstance();
			systems::SystemsManager& sm = gi.GetSystemsManager();
			const auto& archetypes = sm.GetArchetypesOfEntity( GetOwner().GetEntityID() );

			auto found = std::find_if( archetypes.begin(), archetypes.end(), []( auto* archetype ) { return archetype->ContainsData< TData >(); } );
			FORGE_ASSERT( found != archetypes.end() );			
			return (*found)->GetData< TData >( GetOwner().GetEntityID() );
		}

	private:
	};
}

