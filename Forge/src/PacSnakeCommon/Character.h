#pragma once
#include "../GameEngine/SceneObject.h"
#include "Grid.h"

namespace forge
{
	class ObjectInitData;
}

namespace pacsnake
{
	class Character : public forge::SceneObject
	{
		RTTI_DECLARE_ABSTRACT_CLASS( Character, forge::SceneObject );

	public:
		enum class Action
		{
			None,
			TurnLeft,
			TurnRight,
		};

		void ScheduleAction( Action action )
		{
			m_scheduledAction = action;
		}

		const Vector2& GetDir() const;

	protected:
		virtual void OnInit( forge::ObjectInitData& initData ) override;

	private:
		void TurnLeft();
		void TurnRight();

		forge::CallbackToken m_gridUpdateToken;
		forge::CallbackToken m_beforeGridUpdateToken;
		forge::CallbackToken m_updateToken;

		Vector2 m_prevPos;
		Float m_prevPeriod = 0.0f;

		pacsnake::GridPawnID m_pawnID;
		Action m_scheduledAction;
	};
}