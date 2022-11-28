#pragma once
#ifdef FORGE_IMGUI_ENABLED
namespace imgui
{
	class TopBarItem;
}
#endif

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
		forge::DataPackage< T >& GetData()
		{
			FORGE_ASSERT( dynamic_cast< forge::DataPackage< T >* >( m_data.at( &T::GetTypeStatic() ).get() ) );
			return *static_cast< forge::DataPackage< T >* >( m_data.at( &T::GetTypeStatic() ).get() );
		}

		template< class T >
		const T& GetData( forge::EntityID id ) const
		{
			Int32 index = m_sparseSet[ id.m_id ];
			return GetData< T >()[ index ];
		}

		template< class T >
		T& GetData( forge::EntityID id )
		{
			Int32 index = m_sparseSet[ id.m_id ];
			return GetData< T >()[ index ];
		}

		forge::IDataPackage& GetData( const rtti::IType& type )
		{
			return *m_data.at( &type );
		}

		void OnEntityCreated()
		{
			m_sparseSet.emplace_back( -1 );
		}

		virtual void OnEntityDestructed( forge::EntityID id )
		{
			FORGE_ASSERT( m_sparseSet[ id.m_id ] == -1 ); // pls destroy components before you destroy their owner
		}

		Bool ContainsEntity( forge::EntityID id ) const
		{
			return m_sparseSet.at( id.m_id ) >= 0;
		}

		template< class T >
		Bool ContainsData() const
		{
			return ContainsData( T::GetTypeStatic() );
		}

		Bool ContainsData( const rtti::IType& type ) const
		{
			return m_data.count( &type ) > 0u;
		}

		template< class T >
		void AddDataPackage()
		{
			m_data.emplace( &T::GetTypeStatic(), std::make_unique< forge::DataPackage< T > >( m_dataSize ) );
		}

		void AddDataPackage( std::unique_ptr< forge::IDataPackage > package )
		{
			m_data.emplace( &package->GetDataType(), std::move( package ) );
		}

		void SetDirty( Bool dirty )
		{
			m_dirty = dirty;
		}

		Bool IsDirty() const
		{
			return m_dirty;
		}

		Uint32 GetDataSize() const
		{
			return m_dataSize;
		}

		Bool IsEmpty() const
		{
			return GetDataSize() == 0u;
		}

		void MoveEntityTo( forge::EntityID entityId, Archetype* destination );
		void MoveEntityTo( forge::EntityID entityId, std::vector< std::unique_ptr< forge::IDataPackage > >& destination );
		void MoveEntityFrom( forge::EntityID entityId, std::vector< Archetype* > donorArchetypes );

	private:
		std::unordered_map< const rtti::IType*, std::unique_ptr< forge::IDataPackage > > m_data;
		std::vector< Int32 > m_sparseSet;
		Uint32 m_dataSize = 0u;
		Bool m_dirty = false;
	};

	class ISystem
	{
		friend class SystemsManager;
		DECLARE_ABSTRACT_CLASS( ISystem );
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		virtual Bool IsECSSystem() const
		{
			return false;
		}

		forge::EngineInstance& GetEngineInstance() const
		{
			return *m_engineInstance;
		}

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
		virtual std::vector< const rtti::IType* > GetRequiredDataTypes() const = 0;
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

		virtual std::vector< const rtti::IType* > GetRequiredDataTypes() const override
		{
			std::vector< const rtti::IType* > types;
			GatherDataTypesInternal< Ts... >( types );
			return types;
		}

	private:
		template< class... Args >
		static decltype( typename std::enable_if<sizeof...( Args ) == 0, void>::type() ) GatherDataTypesInternal( std::vector< const rtti::IType* >& types )
		{}

		template< class T, class... Args >
		static void GatherDataTypesInternal( std::vector< const rtti::IType* >& types )
		{
			types.emplace_back( &T::GetTypeStatic() );

			GatherDataTypesInternal< Args... >( types );
		}

		template< class... Args >
		static decltype( typename std::enable_if<sizeof...( Args ) == 0, void>::type() ) GatherDataPackagesInternal( std::vector< std::unique_ptr< forge::IDataPackage > >& packages )
		{}

		template< class T, class... Args >
		static void GatherDataPackagesInternal( std::vector< std::unique_ptr< forge::IDataPackage > >& packages )
		{
			packages.emplace_back( std::make_unique< forge::DataPackage< T > >() );

			GatherDataPackagesInternal< Args... >( packages );
		}
	};

	class IECSSystem : public ISystem
	{
		DECLARE_ABSTRACT_CLASS( IECSSystem, systems, ISystem );
	public:
		virtual std::vector< std::unique_ptr< IArchetypeDataTypes > > GetArchetypesDataTypes() const = 0;

		virtual Bool IsECSSystem() const override
		{
			return true;
		}

	private:
		using ISystem::ISystem;
	};

	template< class... Ts >
	class ECSSystem : public IECSSystem
	{
	public:
		using IECSSystem::IECSSystem;

		virtual std::vector< std::unique_ptr< IArchetypeDataTypes > > GetArchetypesDataTypes() const override
		{
			std::vector< std::unique_ptr< IArchetypeDataTypes > > types;
			GatherArchetypesDataTypesInternal< Ts... >( types );
			return types;
		}

	private:
		template< class... Args >
		static decltype( typename std::enable_if<sizeof...( Args ) == 0, void>::type() ) GatherArchetypesDataTypesInternal( std::vector< std::unique_ptr< IArchetypeDataTypes > >& )
		{}

		template< class T, class... Args >
		static void GatherArchetypesDataTypesInternal( std::vector< std::unique_ptr< IArchetypeDataTypes > >& types )
		{
			types.emplace_back( std::make_unique< T >() );

			GatherArchetypesDataTypesInternal< Args... >( types );
		}
	};
}

