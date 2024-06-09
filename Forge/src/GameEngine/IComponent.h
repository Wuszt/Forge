#pragma once
#include "../ECS/EntityID.h"
#include "../ECS/ECSManager.h"
#include "../ECS/CommandsQueue.h"

namespace ecs
{
	class CommandsQueue;
}

namespace forge
{
	class IComponent
	{
		RTTI_DECLARE_ABSTRACT_CLASS( IComponent );
	public:
		friend class Object;

		IComponent();
		virtual ~IComponent() = 0;

		Object& GetOwner() const
		{
			return *m_owner;
		}

	protected:
		virtual void OnAttaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) {}
		virtual void OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) {}

		virtual void OnAttached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) {}
		virtual void OnDetached( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) {}

	private:
		void Attach( EngineInstance& engineInstance, Object& owner, ecs::CommandsQueue& commandsQueue );
		void Detach( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue );

		Object* m_owner;
	};

	namespace datacomponent::internal
	{
		void OnAttaching( forge::Object& owner, const ecs::Fragment::Type& fragmentType, ecs::CommandsQueue& commandsQueue );
		void OnDetaching( forge::Object& owner, const ecs::Fragment::Type& fragmentType, ecs::CommandsQueue& commandsQueue );
		ecs::EntityID GetObjectEntityID( forge::Object& owner );
	}

	class IDataComponent : public IComponent
	{
		RTTI_DECLARE_ABSTRACT_CLASS( IDataComponent, forge::IComponent );

	public:
		virtual const void* GetRawData( const ecs::Fragment::Type*& outDataType ) const = 0;
		virtual void* GetMutableRawData( const ecs::Fragment::Type*& outDataType ) = 0;
	};

	template< class TData, class ParentClass = IDataComponent >
	class DataComponent : public ParentClass
	{
		static_assert( std::is_base_of_v< IDataComponent, ParentClass >, "Parent class has to implement IDataComponent." );
		static_assert( std::is_base_of_v< ecs::Fragment, TData >, "Data type has to derive from Fragment" );

	public:
		using ParentClass::ParentClass;
		using ParentClass::GetOwner;

		virtual void OnAttaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override
		{
			ParentClass::OnAttaching( engineInstance, commandsQueue );
			datacomponent::internal::OnAttaching( GetOwner(), TData::GetTypeStatic(), commandsQueue );
		}

		virtual void OnDetaching( EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue ) override
		{
			ParentClass::OnDetaching( engineInstance, commandsQueue );
			datacomponent::internal::OnDetaching( GetOwner(), TData::GetTypeStatic(), commandsQueue );
		}

		ecs::FragmentView< TData > GetData() const
		{
			auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
			ecs::EntityID id = datacomponent::internal::GetObjectEntityID( GetOwner() );
			return ecsManager.GetFragmentView< TData >( id );
		}

		virtual const void* GetRawData( const ecs::Fragment::Type*& outDataType ) const 
		{
			outDataType = &TData::GetTypeStatic();
			return GetData().GetPtr();
		}

		virtual void* GetMutableRawData( const ecs::Fragment::Type*& outDataType ) 
		{ 
			outDataType = &TData::GetTypeStatic();
			return GetMutableData().GetMutablePtr(); 
		}

	protected:
		virtual ecs::MutableFragmentView< TData > GetMutableData()
		{
			auto& ecsManager = GetOwner().GetEngineInstance().GetECSManager();
			ecs::EntityID id = datacomponent::internal::GetObjectEntityID( GetOwner() );
			return ecsManager.GetMutableFragmentView< TData >( id );
		}

	private:
	};
}