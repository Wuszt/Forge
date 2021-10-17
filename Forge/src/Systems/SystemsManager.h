#pragma once
#include "EntityID.h"
#include "EngineInstance.h" // todo - separate IManager and EngineInstance

namespace forge
{
	class EngineInstance;
}

namespace systems
{
	class ISystem;
	class IECSSystem;
	class Archetype;

	class SystemsManager : public forge::IManager
	{
	public:
		using forge::IManager::IManager;

		virtual void Initialize() override;
		virtual void Deinitialize() override;

		class BootContext
		{
		public:
			using CreationFunc = std::function < std::unique_ptr< ISystem >( forge::EngineInstance& ) >;
			using ArchetypeFunc = std::function < std::unique_ptr< Archetype >() >;

			template< class T >
			FORGE_INLINE void AddSystem()
			{
				m_systemsCreations.emplace_back( []( forge::EngineInstance& engineInstance ) { return std::make_unique< T >( engineInstance ); } );
			}

			FORGE_INLINE const std::vector< CreationFunc >& GetSystemsCreations() const
			{
				return m_systemsCreations;
			}

			FORGE_INLINE const std::vector< ArchetypeFunc >& GetArchetypesCreations() const
			{
				return m_archetypesCreations;
			}

		private:
			std::vector< CreationFunc > m_systemsCreations;
			std::vector< ArchetypeFunc > m_archetypesCreations;
		};

		template< class T >
		T& GetSystem() const
		{
			ISystem* sys = m_systemsLUT.at( typeid( T ) );
			return *static_cast< T* >( sys );
		}

		template< class T >
		T* GetSystemPtr() const
		{
			auto it = m_systemsLUT.find( typeid( T ) );

			if( it != m_systemsLUT.end() )
			{
				return static_cast<T*>( it->second );
			}

			return nullptr;
		}

		template< class T >
		FORGE_INLINE const std::vector< Archetype* >& GetArchetypesWithDataType()
		{
			return m_dataToArchetypesLUT.at( typeid( T ) );
		}

		FORGE_INLINE const std::vector< Archetype* >& GetArchetypesOfEntity( forge::EntityID id )
		{
			return m_entityArchetypesLUT[ id.m_id ];
		}

		template< class T >
		FORGE_INLINE const std::vector< Archetype* >& GetArchetypeOfSystem()
		{
			return m_systemToArchetypesLUT[ typeid( T ) ];
		}

		void AddECSData( forge::EntityID id, std::type_index typeIndex, std::unique_ptr< forge::IDataPackage > package );

		template< class T >
		FORGE_INLINE void AddECSData( forge::EntityID id )
		{
			const std::type_index typeIndex = typeid( T );
			std::unique_ptr< forge::DataPackage< T > > package = std::make_unique< forge::DataPackage< T > >();
			package->AddEmptyData();
			AddECSData( id, typeIndex, std::move( package ) );
		}

		forge::CallbackToken RegisterToOnBootCallback( const std::function< void() >& callback )
		{
			return m_onBootCallback.AddListener( callback );
		}

		void Boot( const BootContext& ctx );

		void Update();

		void Shutdown();

	private:
		std::unordered_map< std::type_index, ISystem* > m_systemsLUT;
		std::vector< std::unique_ptr< ISystem > > m_systems;
		std::vector< std::unique_ptr< IECSSystem > > m_ecsSystems;

		std::vector< std::unique_ptr< Archetype > > m_archetypes;
		std::unordered_map< std::type_index, std::vector< Archetype* > > m_dataToArchetypesLUT;
		std::unordered_map< forge::EntityID, std::vector< std::type_index > > m_entityDataTypesLUT;
		std::unordered_map< forge::EntityID, std::vector< Archetype* > > m_entityArchetypesLUT;
		std::unordered_map< Uint32, Archetype* > m_typesHashToArchetypeLUT;
		std::unordered_map< std::type_index, std::vector< Archetype* > > m_systemToArchetypesLUT;

		std::unique_ptr< forge::CallbackToken > m_onEntityCreated;
		std::unique_ptr< forge::CallbackToken > m_onTick;
		forge::Callback<> m_onBootCallback;
	};
}

