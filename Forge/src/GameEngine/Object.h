#pragma once
#include "../ECS/Fragment.h"

namespace forge
{
	class IComponent;
	class EngineInstance;

	struct ObjectFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( ObjectFragment, ecs::Fragment );

		ObjectID m_objectID;
	};

	class Object
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( Object );

	public:
		Object( EngineInstance& engineInstance, ObjectID id );
		virtual ~Object();

		ObjectID GetObjectID() const
		{
			return m_id;
		}

		virtual void OnAttach();
		virtual void OnDetach();

		template< class T, class... Ts >
		void AddComponents()
		{
			std::vector< std::unique_ptr< IComponent > > createdComponents;

			CreateComponents< T, Ts... >( createdComponents );
			AttachComponents( std::move( createdComponents ) );
		}

		template< class T >
		T* GetComponent()
		{
			return static_cast< T* >( m_components[ m_componentsLUT.at( &T::GetTypeStatic() ) ].get() ); 
		}

		template< class T >
		const T* GetComponent() const
		{
			return static_cast< T* >( m_components[ m_componentsLUT.at( &T::GetTypeStatic() ) ].get() );
		}

		EngineInstance& GetEngineInstance() const
		{
			return *m_engineInstance;
		}

	protected:
		Object();
		Object( Object&& );

	private:
		template< class T >
		std::unique_ptr< T > CreateComponent()
		{
			return std::make_unique< T >();
		}

		template< class... Ts >
		decltype( typename std::enable_if<sizeof...( Ts ) == 0, void>::type() ) CreateComponents( std::vector< std::unique_ptr< IComponent > >& createdComponents )
		{}

		template< class T, class... Ts >
		void CreateComponents( std::vector< std::unique_ptr< IComponent > >& createdComponents)
		{
			createdComponents.emplace_back( CreateComponent< T >() );
			CreateComponents< Ts... >( createdComponents );
		}

		void AttachComponents( std::vector< std::unique_ptr< IComponent > >&& components );

		ObjectID m_id;
		EngineInstance* m_engineInstance = nullptr;

		std::vector< std::unique_ptr< IComponent > > m_components;
		std::unordered_map< const rtti::Type*, Uint32 > m_componentsLUT;
	};
}

