#pragma once
#include "ObjectID.h"
#include "EngineInstance.h" // todo - separate IManager and EngineInstance
#include "ISystem.h"

namespace forge
{
	class EngineInstance;
}

namespace systems
{
	class SystemsManager
	{
	public:
		SystemsManager( forge::EngineInstance& engineInstance );
		~SystemsManager();

		void AddSystems( forge::ArraySpan< const ISystem::ClassType* > systemsClasses );

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

		void Shutdown();

	private:
		std::unordered_map< const ISystem::ClassType*, ISystem* > m_systemsLUT;
		std::vector< std::unique_ptr< ISystem > > m_systems;

		forge::EngineInstance& m_engineInstance;
	};
}

