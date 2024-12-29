#include "Fpch.h"
#include "GridSystem.h"
#include "../GameEngine/UpdateManager.h"
#include "Pickup.h"

RTTI_IMPLEMENT_TYPE( pacsnake::GridSystem );

void pacsnake::GridSystem::OnInitialize()
{
	m_lastSimUpdateTime = forge::Time::GetTime();
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update,
		[ this ]()
		{
			const Float currTime = forge::Time::GetTime();
			if ( currTime > m_lastSimUpdateTime + m_period )
			{
				m_onBeforeSimUpdate.Invoke();
				m_grid.Update();
				m_onSimUpdated.Invoke();

				m_lastSimUpdateTime = currTime;
				m_period -= 0.001f;
				m_period = Math::Max( 0.1f, m_period );

				auto collisions = m_grid.FindCollisions();
				for ( const auto& collision : collisions )
				{
					if ( collision.m_first == m_pickupID || collision.m_second == m_pickupID )
					{
						CreateNewPickup();
					}
				}
			}
		} );

	GetEngineInstance().GetObjectsManager().RequestCreatingObject< pacsnake::Pickup >( { .m_postInitFunc = [ & ]( forge::Object& pickup, forge::ObjectInitData& )
		{
			m_pickupID = rtti::CastChecked< pacsnake::Pickup >( pickup ).GetPawnID();
			CreateNewPickup();
		} } );
}

forge::CallbackToken pacsnake::GridSystem::RegisterOnSimUpdate( std::function< void() > func )
{
	return m_onSimUpdated.AddListener( std::move( func) );
}

forge::CallbackToken pacsnake::GridSystem::RegisterOnBeforeSimUpdate( std::function< void() > func )
{
	return m_onBeforeSimUpdate.AddListener( std::move( func ) );
}

void pacsnake::GridSystem::CreateNewPickup()
{
	auto* pawn = m_grid.GetPawn( m_pickupID );
	const Int32 maxX = static_cast< Int32 >( m_grid.GetWidth() / 2u );
	const Int32 maxY = static_cast< Int32 >( m_grid.GetHeight() / 2u );

	Bool collides = true;
	while ( collides )
	{
		pawn->m_pos = Vector2( static_cast< Float >( Math::Random::GetRNG().GetInteger( -maxX, maxX ) ), static_cast< Float >( Math::Random::GetRNG().GetInteger( -maxY, maxY ) ) );
		collides = false;

		auto collisions = m_grid.FindCollisions();
		Bool collides = false;
		for ( const auto& collision : collisions )
		{
			if ( collision.m_first == m_pickupID || collision.m_second == m_pickupID )
			{
				collides = true;
				break;
			}
		}
	}
}