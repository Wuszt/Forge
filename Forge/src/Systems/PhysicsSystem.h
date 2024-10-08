#pragma once
#include "../GameEngine/ISystem.h"

namespace physics
{
	class PhysxProxy;
	class PhysicsScene;
	class PhysicsActor;
	struct RaycastResult;

	enum class PhysicsGroupFlags : Uint32
	{
		Default = 1u << 0,
		Editor = 1u << 1,
		All = std::numeric_limits< Uint32 >::max(),
	};

	constexpr PhysicsGroupFlags operator&( PhysicsGroupFlags x, PhysicsGroupFlags y )
	{
		return static_cast< PhysicsGroupFlags >( static_cast< Uint32 >( x ) & static_cast< Uint32 >( y ) );
	}

	constexpr PhysicsGroupFlags operator~( PhysicsGroupFlags x )
	{
		return static_cast< PhysicsGroupFlags >( ~static_cast< Uint32 >( x ) );
	}

	constexpr PhysicsGroupFlags operator|( PhysicsGroupFlags x, PhysicsGroupFlags y )
	{
		return static_cast< PhysicsGroupFlags >( static_cast< Uint32 >( x ) | static_cast< Uint32 >( y ) );
	}
}

namespace systems
{
	class PhysicsSystem : public ISystem
	{
		RTTI_DECLARE_POLYMORPHIC_CLASS( PhysicsSystem, systems::ISystem );

	public:
		PhysicsSystem();
		PhysicsSystem( PhysicsSystem&& );
		~PhysicsSystem();

		virtual void OnInitialize() override;
		virtual void OnDeinitialize() override;

		void RegisterActor( physics::PhysicsActor& actor );
		void UnregisterActor( physics::PhysicsActor& actor );

		bool PerformRaycast( const Vector3& start, const Vector3& direction, Float length, physics::RaycastResult& outResult, physics::PhysicsGroupFlags flags = physics::PhysicsGroupFlags::All );

		physics::PhysxProxy& GetPhysicsProxy()
		{
			return *m_physicsProxy;
		}

	private:
		void UpdateSimulation();
		void UpdateScene();
		std::unique_ptr< physics::PhysxProxy > m_physicsProxy;
		std::unique_ptr< physics::PhysicsScene > m_scene;
		forge::CallbackToken m_updateToken;
	};
}