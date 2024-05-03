#pragma once
#include "../ECS/Fragment.h"

namespace forge
{
	class IComponent;
	class EngineInstance;

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

		virtual void OnAttach();
		virtual void OnDetach();

		virtual void PostInit() {}

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
			auto it = m_componentsLUT.find( &T::GetTypeStatic() );
			if ( it == m_componentsLUT.end() )
			{
				return nullptr;
			}

			return static_cast< T* >( m_components[ it->second ].get() ); 
		}

		template< class T >
		const T* GetComponent() const
		{
			return const_cast< This* >( this )->GetComponent< T >();
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
			return m_name.c_str();
		}

	protected:
		Object();
		Object( Object&& );

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

		void AttachComponents( std::vector< std::unique_ptr< IComponent > >&& components );

		std::string m_name;
		std::vector< std::unique_ptr< IComponent > > m_components;
		std::unordered_map< const rtti::Type*, Uint32 > m_componentsLUT;
		EngineInstance* m_engineInstance = nullptr;
		ObjectID m_id;

		friend class forge::ObjectsManager;
	};
}

