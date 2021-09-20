#pragma once

namespace forge
{
	class IComponent;
	class EntitiesManager;

	class Entity
	{
	public:
		Entity( EntitiesManager& entitiesManager, EntityID id );
		virtual ~Entity() {}

		FORGE_INLINE EntityID GetEntityID() const
		{
			return m_id;
		}

		virtual void OnAttach() {}
		virtual void OnDetach();

		template< class T >
		T* AddComponent()
		{
			auto comp = std::make_unique< T >( *this );
			auto* rawcomp = comp.get();
			m_components.emplace( typeid( T ), std::move( comp ) );
			rawcomp->OnAttach();
			return rawcomp;
		}

		template< class T >
		T* GetComponent()
		{
			return static_cast< T* >( m_components.at( typeid( T ) ).get() ); 
		}

		EntitiesManager& GetEntitiesManager() const
		{
			return m_entitiesManager;
		}

	private:
		EntityID m_id;
		EntitiesManager& m_entitiesManager;

		std::unordered_map< std::type_index, std::unique_ptr< IComponent > > m_components;
	};
}

