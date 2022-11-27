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

		EntityID GetEntityID() const
		{
			return m_id;
		}

		virtual void OnAttach() {}
		virtual void OnDetach();

		template< class T, class... Ts >
		void RequestAddingComponents( const std::function< void() >& initializeFunc = nullptr )
		{
			RequestAddingComponentsInternal( [ this, initializeFunc ]()
			{
				AddComponents< T, Ts... >();
				if( initializeFunc )
				{
					initializeFunc();
				}
			} );
		}

		template< class T >
		T* GetComponent()
		{
			return static_cast< T* >( m_components.at( &T::GetTypeStatic() ).get() ); 
		}

		EngineInstance& GetEngineInstance() const
		{
			return m_engineInstance;
		}

	private:
		template< class T >
		T* AddComponent()
		{
			auto comp = std::make_unique< T >();
			auto* rawcomp = comp.get();
			m_components.emplace( &T::GetTypeStatic(), std::move( comp ) );
			rawcomp->Attach( m_engineInstance, *this );
			return rawcomp;
		}

		template< class... Ts >
		decltype( typename std::enable_if<sizeof...( Ts ) == 0, void>::type() ) AddComponents()
		{}

		template< class T, class... Ts >
		void AddComponents()
		{
			AddComponent< T >();
			AddComponents< Ts... >();
		}

		void RequestAddingComponentsInternal( const std::function< void() >& creationFunc );

		EntityID m_id;
		EngineInstance& m_engineInstance;

		std::unordered_map< const rtti::IType*, std::unique_ptr< IComponent > > m_components;
	};
}

