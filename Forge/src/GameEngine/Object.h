#pragma once
#include "../ECS/Fragment.h"
#include "IComponent.h"

namespace forge
{
	class IComponent;
	class EngineInstance;
	class ObjectInitData;

	struct ObjectFragment : public ecs::Fragment
	{
		RTTI_DECLARE_STRUCT( ObjectFragment, ecs::Fragment );

		ObjectFragment() = default;
		ObjectFragment( ObjectID objectId )
			: m_objectID( objectId )
		{}

		ObjectID m_objectID;
	};

	class Object
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( Object );

	public:
		virtual ~Object();

		ObjectID GetObjectID() const
		{
			return m_id;
		}

		virtual void OnInit( ObjectInitData& initData );
		virtual void OnDeinit();

		virtual void PostInit() {}

		template< class T, class... Ts >
		void AddComponents()
		{
			std::vector< std::unique_ptr< IComponent > > createdComponents;

			CreateComponents< T, Ts... >( createdComponents );
			AttachComponents( createdComponents, nullptr );
		}

		template< class T >
		Bool AddComponent()
		{
			return AddComponent( T::GetTypeStatic() );
		}

		Bool AddComponent( const forge::IComponent::Type& componentType );

		void RemoveComponent( const forge::IComponent::Type& componentType );

		template< class T >
		void RemoveComponent()
		{
			RemoveComponent( T::GetTypeStatic() );
		}

		forge::IComponent* GetComponent( const IComponent::Type& componentType )
		{
			auto it = m_componentsLUT.find( &componentType );
			if ( it == m_componentsLUT.end() )
			{
				return nullptr;
			}

			return m_components[ it->second ].get();
		}

		template< class T >
		T* GetComponent()
		{
			return static_cast< T* >( GetComponent( T::GetTypeStatic() ) ); 
		}

		std::vector< IComponent* > GetComponents()
		{
			std::vector< IComponent* > components;
			for ( auto& comp : m_components )
			{
				components.emplace_back( comp.get() );
			}

			return components;
		}

		template< class T >
		const T* GetComponent() const
		{
			return const_cast< This* >( this )->GetComponent< T >();
		}

		Bool HasComponent( const IComponent::Type& componentType ) const
		{
			return m_componentsLUT.contains( &componentType );
		}

		EngineInstance& GetEngineInstance() const
		{
			return *m_engineInstance;
		}

		void SetName( std::string name )
		{
			m_name = std::move( name );
		}

		const char* GetName() const
		{
			return m_name.empty() ? "None" : m_name.c_str();
		}

		virtual void Serialize( forge::Serializer& serializer );
		virtual void Deserialize( forge::Deserializer& deserializer, ObjectInitData& initData );

	protected:
		Object();
		Object( Object&& );

		virtual void OnComponentAttached( IComponent& component ) {}
		virtual void OnComponentDetached( IComponent& component ) {}

	private:
		void Initialize( EngineInstance& engineInstance, ObjectID id )
		{
			m_engineInstance = &engineInstance;
			m_id = id;
			m_name = forge::String::Printf( "_%lu_", static_cast< Uint32 >( id ) );
		}

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

		void AttachComponents( forge::ArraySpan< std::unique_ptr< IComponent > > components, forge::ObjectInitData* initData );

		std::string m_name;
		std::vector< std::unique_ptr< IComponent > > m_components;
		std::unordered_map< const IComponent::Type*, Uint32 > m_componentsLUT;
		EngineInstance* m_engineInstance = nullptr;
		ObjectID m_id;

		friend class forge::ObjectsManager;
	};
}

