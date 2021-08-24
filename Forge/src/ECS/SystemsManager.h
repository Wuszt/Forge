#pragma once
#include <typeindex>

namespace ecs
{
	class ISystem;

	class SystemsManager
	{
	public:
		SystemsManager() = default;
		~SystemsManager() = default;

		class BootContext
		{
		public:
			using CreationFunc = std::function < std::unique_ptr< ISystem >() >;
			template< class T >
			void AddSystem()
			{
				m_systemsCreations.emplace_back( []() { return std::make_unique< T >(); } );
			}

			FORGE_INLINE const std::vector< CreationFunc >& GetSystemsCreations() const
			{
				return m_systemsCreations;
			}

		private:
			std::vector< CreationFunc > m_systemsCreations;
		};

		template< class T >
		T& GetSystem()
		{
			ISystem* sys = m_systems[ m_systemsLUT[ std::type_index( typeid( T ) ) ] ].get();
			return *static_cast< T* >( sys );
		}

		void Boot( const BootContext& ctx );

		void Update();

		void Shutdown();

	private:

		std::unordered_map< std::type_index, Uint32 > m_systemsLUT;
		std::vector< std::unique_ptr< ISystem > > m_systems;
	};
}

