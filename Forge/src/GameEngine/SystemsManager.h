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

		class BootContext
		{
		public:
			template< class T >
			void AddSystem()
			{
				const ISystem::ClassType& type = T::GetTypeStatic();
				FORGE_ASSERT( type.InheritsFrom< ISystem >() && !type.IsAbstract() );
				m_systemsClasses.emplace_back( &type );
			}

			const std::vector< const ISystem::ClassType* >& GetSystemsClasses() const
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

		forge::CallbackToken RegisterToOnBootCallback( const std::function< void() >& callback )
		{
			return m_onBootCallback.AddListener( callback );
		}

		void Boot( const BootContext& ctx );

		void Shutdown();

	private:
		std::unordered_map< const rtti::IType*, ISystem* > m_systemsLUT;
		std::vector< std::unique_ptr< ISystem > > m_allSystems;

		forge::Callback<> m_onBootCallback;
		forge::EngineInstance& m_engineInstance;
	};
}

