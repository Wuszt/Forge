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

		FORGE_INLINE void OnEntityCreated()
		{
			m_sparseSet.emplace_back( -1 );
		}

		FORGE_INLINE virtual void OnEntityDestructed( forge::EntityID id )
		{
			FORGE_ASSERT( m_sparseSet[ id.m_id ] == -1 ); // pls destroy components before you destroy their owner
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

		FORGE_INLINE void SetDirty( Bool dirty )
		{
			m_dirty = dirty;
		}

		FORGE_INLINE Bool IsDirty() const
		{
			return m_dirty;
		}

		FORGE_INLINE Uint32 GetDataSize() const
		{
			return m_dataSize;
		}

		FORGE_INLINE Bool IsEmpty() const
		{
			return GetDataSize() == 0u;
		}

		void MoveEntityTo( forge::EntityID entityId, Archetype* destination );
		void MoveEntityTo( forge::EntityID entityId, std::vector< std::unique_ptr< forge::IDataPackage > >& destination );
		void MoveEntityFrom( forge::EntityID entityId, std::vector< Archetype* > donorArchetypes );

	private:
		std::unordered_map< std::type_index, std::unique_ptr< forge::IDataPackage > > m_data;
		std::vector< Int32 > m_sparseSet;
		Uint32 m_dataSize = 0u;
		Bool m_dirty = false;
	};

	class ISystem
	{
		friend class SystemsManager;
		DECLARE_ABSTRACT_TYPE( ISystem );
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		FORGE_INLINE forge::EngineInstance& GetEngineInstance() const
		{
			return *m_engineInstance;
		}

#ifdef FORGE_DEBUGGING
		virtual const std::string& GetDebugFriendlyName() const { static std::string emptyName; return emptyName; }
		virtual void OnRenderDebug() {}
#endif

	protected:
		virtual void OnInitialize() {}
		virtual void OnDeinitialize() {}

	private:
		void Initialize( forge::EngineInstance& engineInstance );
		void Deinitialize();
		forge::EngineInstance* m_engineInstance;
	};

	class IArchetypeDataTypes
	{
	public:
		virtual ~IArchetypeDataTypes() = default;

		virtual std::vector< std::unique_ptr< forge::IDataPackage > > GatherDataPackages() const = 0;
		virtual std::vector< std::type_index > GetRequiredDataTypes() const = 0;
	};

	template< class... Ts >
	class ArchetypeDataTypes : public IArchetypeDataTypes
	{
	public:
		virtual std::vector< std::unique_ptr< forge::IDataPackage > > GatherDataPackages() const override
		{
			std::vector< std::unique_ptr< forge::IDataPackage > > packages;
			GatherDataPackagesInternal< Ts... >( packages );
			return packages;
		}

		virtual std::vector< std::type_index > GetRequiredDataTypes() const override
		{
			std::vector< std::type_index > types;
			GatherDataTypesInternal< Ts... >( types );
			return types;
		}

	private:
		template< class... Args >
		FORGE_INLINE static decltype( typename std::enable_if<sizeof...( Args ) == 0, void>::type() ) GatherDataTypesInternal( std::vector< std::type_index >& types )
		{}

		template< class T, class... Args >
		FORGE_INLINE static void GatherDataTypesInternal( std::vector< std::type_index >& types )
		{
			types.emplace_back( typeid( T ) );

			GatherDataTypesInternal< Args... >( types );
		}

		template< class... Args >
		FORGE_INLINE static decltype( typename std::enable_if<sizeof...( Args ) == 0, void>::type() ) GatherDataPackagesInternal( std::vector< std::unique_ptr< forge::IDataPackage > >& packages )
		{}

		template< class T, class... Args >
		FORGE_INLINE static void GatherDataPackagesInternal( std::vector< std::unique_ptr< forge::IDataPackage > >& packages )
		{
			packages.emplace_back( std::make_unique< forge::DataPackage< T > >() );

			GatherDataPackagesInternal< Args... >( packages );
		}
	};

	class IECSSystem : public ISystem
	{
	public:
		virtual std::vector< std::unique_ptr< IArchetypeDataTypes > > GetArchetypesDataTypes() const = 0;

	private:
		using ISystem::ISystem;
	};

	template< class... Ts >
	class ECSSystem : public IECSSystem
	{
	public:
		using IECSSystem::IECSSystem;

		FORGE_INLINE virtual std::vector< std::unique_ptr< IArchetypeDataTypes > > GetArchetypesDataTypes() const override
		{
			std::vector< std::unique_ptr< IArchetypeDataTypes > > types;
			GatherArchetypesDataTypesInternal< Ts... >( types );
			return types;
		}

	private:
		template< class... Args >
		FORGE_INLINE static decltype( typename std::enable_if<sizeof...( Args ) == 0, void>::type() ) GatherArchetypesDataTypesInternal( std::vector< std::unique_ptr< IArchetypeDataTypes > >& )
		{}

		template< class T, class... Args >
		FORGE_INLINE static void GatherArchetypesDataTypesInternal( std::vector< std::unique_ptr< IArchetypeDataTypes > >& types )
		{
			types.emplace_back( std::make_unique< T >() );

			GatherArchetypesDataTypesInternal< Args... >( types );
		}
	};
}

