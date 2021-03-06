#pragma once
#include "EntityID.h"
#include "EngineInstance.h" // todo - separate IManager and EngineInstance
#include "ISystem.h"

namespace forge
{
	class EngineInstance;
}

namespace systems
{
	class IECSSystem;
	class Archetype;
	class IArchetypeDataTypes;

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
				const ISystem::ClassType& type = T::GetTypeStatic();
				FORGE_ASSERT( type.InheritsFrom< ISystem >() && !type.IsAbstract() );
				m_systemsClasses.emplace_back( &type );
			}

			FORGE_INLINE const std::vector< const ISystem::ClassType* >& GetSystemsClasses() const
			{
				return m_systemsClasses;
			}

		private:
			std::vector< const ISystem::ClassType* > m_systemsClasses;
		};

		template< class T >
		T& GetSystem() const
		{
			ISystem* sys = m_systemsLUT.at( &T::GetTypeStatic() );
			return *static_cast< T* >( sys );
		}

		template< class T >
		T* GetSystemPtr() const
		{
			auto it = m_systemsLUT.find( &T::GetTypeStatic() );

			if( it != m_systemsLUT.end() )
			{
				return static_cast<T*>( it->second );
			}

			return nullptr;
		}

		forge::ArraySpan< IECSSystem* > GetECSSystems()
		{
			return m_ecsSystems;
		}

		forge::ArraySpan< std::unique_ptr< ISystem > > GetSystems()
		{
			return m_allSystems;
		}

		template< class T >
		FORGE_INLINE forge::ArraySpan< systems::Archetype* > GetArchetypesContainingDataType()
		{
			return m_dataToArchetypesLUT[ &T::GetTypeStatic() ];
		}

		FORGE_INLINE const std::vector< Archetype* >& GetArchetypesOfEntity( forge::EntityID id )
		{
			return m_entityArchetypesLUT[ id.m_id ];
		}

		forge::ArraySpan< systems::Archetype* > GetArchetypesWithDataTypes( const IArchetypeDataTypes& archetypeDataTypes );

		void AddECSData( forge::EntityID id, std::unique_ptr< forge::IDataPackage > package );

		template< class T >
		FORGE_INLINE void AddECSData( forge::EntityID id )
		{
			std::unique_ptr< forge::DataPackage< T > > package = std::make_unique< forge::DataPackage< T > >();
			package->AddEmptyData();
			AddECSData( id, std::move( package ) );
		}

		void RemoveECSData( forge::EntityID id, const rtti::IType& type );

		template< class T >
		FORGE_INLINE void RemoveECSData( forge::EntityID id )
		{
			RemoveECSData( id, T::GetTypeStatic() );
		}

		forge::CallbackToken RegisterToOnBootCallback( const std::function< void() >& callback )
		{
			return m_onBootCallback.AddListener( callback );
		}

		void Boot( const BootContext& ctx );

		void Shutdown();

	private:
		std::unordered_map< const rtti::IType*, ISystem* > m_systemsLUT;
		std::vector< std::unique_ptr< ISystem > > m_allSystems;
		std::vector< IECSSystem* > m_ecsSystems;

		std::vector< std::unique_ptr< Archetype > > m_archetypes;
		std::unordered_map< const rtti::IType*, std::vector< Archetype* > > m_dataToArchetypesLUT;
		std::unordered_map< forge::EntityID, std::vector< const rtti::IType* > > m_entityDataTypesLUT;
		std::unordered_map< forge::EntityID, std::vector< Archetype* > > m_entityArchetypesLUT;
		std::unordered_map< Uint32, Archetype* > m_typesHashToArchetypeLUT;
		std::unordered_map< Uint32, std::vector< Archetype* > > m_archetypeTypesHashToArchetypesLUT;

		forge::CallbackToken m_onEntityCreated;
		forge::CallbackToken m_onEntityDestructed;
		forge::CallbackToken m_onTick;
		forge::Callback<> m_onBootCallback;
	};
}

