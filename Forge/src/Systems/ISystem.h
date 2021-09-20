#pragma once

namespace systems
{
	class IDataPackage
	{
	public:
		virtual ~IDataPackage() = default;
		virtual void AddEmptyData() = 0;
		virtual void RemoveDataReorder( Uint32 index ) = 0;
		virtual std::type_index GetTypeIndex() const = 0;
		virtual void MoveTo( Uint32 index, IDataPackage& destination ) = 0;
	};

	template< class T >
	class DataPackage : public IDataPackage
	{
	public:
		DataPackage( Uint32 initialSize = 0u )
			: m_data( initialSize )
		{}

		FORGE_INLINE T& operator[]( Uint32 index )
		{
			return m_data[ index ];
		}

		FORGE_INLINE virtual void AddEmptyData() override
		{
			AddData();
		}

		FORGE_INLINE virtual void RemoveDataReorder( Uint32 index ) override
		{
			std::swap( m_data[ index ], m_data.back() );
			m_data.pop_back();
		}

		FORGE_INLINE virtual std::type_index GetTypeIndex() const override
		{
			return typeid( T );
		}

		FORGE_INLINE virtual void MoveTo( Uint32 index, IDataPackage& destination ) override
		{
			FORGE_ASSERT( dynamic_cast< DataPackage< T >* >( &destination ) );
			DataPackage< T >* realType = static_cast< DataPackage< T >* >( &destination );
			realType->m_data.emplace_back( std::move( m_data[ index ] ) );
			RemoveDataReorder( index );
		}

		template< class... Ts >
		FORGE_INLINE void AddData( Ts... data )
		{
			m_data.emplace_back( std::forward< Ts >( data )... );
		}

	private:
		std::vector< T > m_data;
	};


	class Archetype
	{
	public:
		Archetype( Uint32 size = 0u )
			: m_sparseSet( size + 1, -1 )
		{}

		template< class T >
		FORGE_INLINE DataPackage< T >& GetData()
		{
			FORGE_ASSERT( dynamic_cast< DataPackage< T >* >( m_data.at( typeid( T ) ).get() ) );
			return *static_cast< DataPackage< T >* >( m_data.at( typeid( T ) ).get() );
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

		FORGE_INLINE IDataPackage& GetData( std::type_index typeIndex )
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
			m_data.emplace( typeid( T ), std::make_unique< DataPackage< T > >( m_dataSize ) );
		}

		FORGE_INLINE void AddDataPackage( std::unique_ptr< IDataPackage > package )
		{
			m_data.emplace( package->GetTypeIndex(), std::move( package ) );
		}

		void MoveEntityTo( forge::EntityID entityId, Archetype* destination );
		void MoveEntityFrom( forge::EntityID entityId, std::vector< Archetype* > donorArchetypes );

		void AddEntity( forge::EntityID id );
		void RemoveEntity( forge::EntityID id );

	private:
		std::unordered_map< std::type_index, std::unique_ptr< IDataPackage > > m_data;
		std::vector< Int32 > m_sparseSet;
		Uint32 m_dataSize = 0u;
	};

	class ISystem
	{
	public:
		ISystem( forge::GameInstance& gameInstance )
			: m_gameInstance( gameInstance )
		{}

		virtual ~ISystem() = default;

		virtual void OnInitialize() {}
		virtual void Update() {}
		virtual void OnDeinitialize() {}

		FORGE_INLINE forge::GameInstance& GetGameInstance() const
		{
			return m_gameInstance;
		}

	private:
		forge::GameInstance& m_gameInstance;
	};

	class IECSSystem : public ISystem
	{
	public:
		virtual const std::vector< std::type_index >& GetRequiredDataTypes() const = 0;
		virtual std::vector< std::unique_ptr< IDataPackage > > CreateRequiredDataPackages() const = 0;

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
		FORGE_INLINE static decltype( typename std::enable_if<sizeof...( Args ) == 0, void>::type() ) GatherDataPackages( std::vector< std::unique_ptr< IDataPackage > >& packages )
		{}

		template< class T, class... Args >
		FORGE_INLINE static void GatherDataPackages( std::vector< std::unique_ptr< IDataPackage > >& packages )
		{
			packages.emplace_back( std::make_unique< DataPackage< T > >() );

			GatherDataPackages< Args... >( packages );
		}

		FORGE_INLINE virtual std::vector< std::unique_ptr< IDataPackage > > CreateRequiredDataPackages() const override
		{
			std::vector< std::unique_ptr< IDataPackage > > packages;
			GatherDataPackages< Ts... >( packages );
			return packages;
		}
	};
}

