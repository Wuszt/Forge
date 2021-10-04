#pragma once

namespace forge
{
	class EngineInstance;
}

namespace systems
{
	class Archetype
	{
	public:
		Archetype( Uint32 size = 0u )
			: m_sparseSet( size + 1, -1 )
		{}

		template< class T >
		FORGE_INLINE forge::DataPackage< T >& GetData()
		{
			FORGE_ASSERT( dynamic_cast< forge::DataPackage< T >* >( m_data.at( typeid( T ) ).get() ) );
			return *static_cast< forge::DataPackage< T >* >( m_data.at( typeid( T ) ).get() );
		}

		template< class T >
		FORGE_INLINE const T& GetData( forge::EntityID id ) const
		{
			Int32 index = m_sparseSet[ id.m_id ];
			return GetData< T >()[ index ];
		}

		template< class T >
		FORGE_INLINE T& GetData( forge::EntityID id )
		{
			Int32 index = m_sparseSet[ id.m_id ];
			return GetData< T >()[ index ];
		}

		FORGE_INLINE forge::IDataPackage& GetData( std::type_index typeIndex )
		{
			return *m_data.at( typeIndex );
		}

		FORGE_INLINE void OnEntityCreation()
		{
			m_sparseSet.emplace_back( -1 );
		}

		FORGE_INLINE virtual void OnEntityDestruction( forge::EntityID id )
		{
			FORGE_ASSERT( m_sparseSet[ id.m_id ] != -1 ); // pls destroy components before you destroy their owner
		}

		FORGE_INLINE Bool ContainsEntity( forge::EntityID id ) const
		{
			return m_sparseSet.at( id.m_id ) >= 0;
		}

		template< class T >
		FORGE_INLINE Bool ContainsData() const
		{
			return ContainsData( typeid( T ) );
		}

		FORGE_INLINE Bool ContainsData( std::type_index typeIndex ) const
		{
			return m_data.count( typeIndex ) > 0u;
		}

		template< class T >
		FORGE_INLINE void AddDataPackage()
		{
			m_data.emplace( typeid( T ), std::make_unique< forge::DataPackage< T > >( m_dataSize ) );
		}

		FORGE_INLINE void AddDataPackage( std::unique_ptr< forge::IDataPackage > package )
		{
			m_data.emplace( package->GetTypeIndex(), std::move( package ) );
		}

		void MoveEntityTo( forge::EntityID entityId, Archetype* destination );
		void MoveEntityFrom( forge::EntityID entityId, std::vector< Archetype* > donorArchetypes );

		void AddEntity( forge::EntityID id );
		void RemoveEntity( forge::EntityID id );

	private:
		std::unordered_map< std::type_index, std::unique_ptr< forge::IDataPackage > > m_data;
		std::vector< Int32 > m_sparseSet;
		Uint32 m_dataSize = 0u;
	};

	class ISystem
	{
	public:
		ISystem( forge::EngineInstance& engineInstance )
			: m_engineInstance( engineInstance )
		{}

		virtual ~ISystem() = default;

		virtual void OnInitialize() {}
		virtual void Update() {}
		virtual void OnDeinitialize() {}

		FORGE_INLINE forge::EngineInstance& GetEngineInstance() const
		{
			return m_engineInstance;
		}

	private:
		forge::EngineInstance& m_engineInstance;
	};

	class IECSSystem : public ISystem
	{
	public:
		virtual const std::vector< std::type_index >& GetRequiredDataTypes() const = 0;
		virtual std::vector< std::unique_ptr< forge::IDataPackage > > CreateRequiredDataPackages() const = 0;

	private:
		using ISystem::ISystem;
	};

	template< class... Ts>
	class ECSSystem : public IECSSystem
	{
	public:
		using IECSSystem::IECSSystem;

		template< class... Args >
		FORGE_INLINE static decltype( typename std::enable_if<sizeof...( Args ) == 0, void>::type() ) GatherDataTypes( std::vector< std::type_index >& types )
		{}

		template< class T, class... Args >
		FORGE_INLINE static void GatherDataTypes( std::vector< std::type_index >& types )
		{
			types.emplace_back( typeid( T ) );

			GatherDataTypes< Args... >( types );
		}

		FORGE_INLINE static std::vector< std::type_index > ComputeDataTypes()
		{
			std::vector< std::type_index > types;
			GatherDataTypes< Ts... >( types );
			return types;
		}

		FORGE_INLINE static const std::vector< std::type_index >& GetRequiredDataTypesStatic()
		{
			thread_local std::vector< std::type_index > types = ComputeDataTypes();
			return types;
		}

		FORGE_INLINE virtual const std::vector< std::type_index >& GetRequiredDataTypes() const override
		{
			return GetRequiredDataTypesStatic();
		}

		template< class... Args >
		FORGE_INLINE static decltype( typename std::enable_if<sizeof...( Args ) == 0, void>::type() ) GatherDataPackages( std::vector< std::unique_ptr< forge::IDataPackage > >& packages )
		{}

		template< class T, class... Args >
		FORGE_INLINE static void GatherDataPackages( std::vector< std::unique_ptr< forge::IDataPackage > >& packages )
		{
			packages.emplace_back( std::make_unique< forge::DataPackage< T > >() );

			GatherDataPackages< Args... >( packages );
		}

		FORGE_INLINE virtual std::vector< std::unique_ptr< forge::IDataPackage > > CreateRequiredDataPackages() const override
		{
			std::vector< std::unique_ptr< forge::IDataPackage > > packages;
			GatherDataPackages< Ts... >( packages );
			return packages;
		}
	};
}

