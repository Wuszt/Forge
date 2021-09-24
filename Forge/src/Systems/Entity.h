#pragma once

namespace forge
{
	class IComponent;
	class EngineInstance;

	class Entity
	{
	public:
		Entity( EngineInstance& engineInstance, EntityID id );
		virtual ~Entity();

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
			rawcomp->OnAttach( m_engineInstance );
			return rawcomp;
		}

		template< class... Ts >
		FORGE_INLINE decltype( typename std::enable_if<sizeof...( Ts ) == 0, void>::type() ) AddComponents()
		{}

		template< class T, class... Ts >
		void AddComponents()
		{
			AddComponent< T >();
			AddComponents< Ts... >();
		}

		template< class T >
		T* GetComponent()
		{
			return static_cast< T* >( m_components.at( typeid( T ) ).get() ); 
		}

		EngineInstance& GetEngineInstance() const
		{
			return m_engineInstance;
		}

	private:
		EntityID m_id;
		EngineInstance& m_engineInstance;

		std::unordered_map< std::type_index, std::unique_ptr< IComponent > > m_components;
	};
}

