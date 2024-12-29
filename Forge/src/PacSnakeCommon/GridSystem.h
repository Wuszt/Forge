#pragma once
#include "../GameEngine/ISystem.h"
#include "../PacSnakeCommon/Grid.h"

namespace pacsnake
{
	class GridSystem : public systems::ISystem
	{
		RTTI_DECLARE_CLASS( GridSystem, systems::ISystem );

	public:
		GridSystem()
			: m_grid( Grid( 19u, 19u ) )
		{}

		forge::CallbackToken RegisterOnSimUpdate( std::function< void() > func );
		forge::CallbackToken RegisterOnBeforeSimUpdate( std::function< void() > func );

		Grid& GetGrid()
		{
			return m_grid;
		}

		Float GetPeriod() const
		{
			return m_period;
		}

		Float GetLastSimUpdateTime() const
		{
			return m_lastSimUpdateTime;
		}

	private:
		virtual void OnInitialize() override;
		void CreateNewPickup();

		Grid m_grid;
		forge::CallbackToken m_updateToken;
		forge::Callback<> m_onSimUpdated;
		forge::Callback<> m_onBeforeSimUpdate;
		pacsnake::GridPawnID m_pickupID;

		Float m_lastSimUpdateTime = 0.0f;
		Float m_period = 0.5f;
	};
}