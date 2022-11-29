#pragma once

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
		virtual ~IComponent();

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
			auto& systemsManager = engineInstance.GetSystemsManager();

			systemsManager.AddECSData< TData >( GetOwner().GetObjectID() );
		}

		virtual void OnDetach( EngineInstance& engineInstance ) override
		{
			auto& systemsManager = engineInstance.GetSystemsManager();

			systemsManager.RemoveECSData< TData >( GetOwner().GetObjectID() );
		}

		TData& GetData()
		{
			auto& ei = GetOwner().GetEngineInstance();
			systems::SystemsManager& sm = ei.GetSystemsManager();
			const auto& archetypes = sm.GetArchetypesOfObject( GetOwner().GetObjectID() );

			auto found = std::find_if( archetypes.begin(), archetypes.end(), []( auto* archetype ) { return archetype->ContainsData< TData >(); } );
			if( found == archetypes.end() )
			{
				found = found;
			}
			FORGE_ASSERT( found != archetypes.end() );
			return ( *found )->GetData< TData >( GetOwner().GetObjectID() );
		}

	private:
	};
}