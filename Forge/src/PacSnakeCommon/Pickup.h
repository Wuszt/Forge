#pragma once
#include "../GameEngine/SceneObject.h"
#include "Grid.h"

namespace forge
{
	class ObjectInitData;
}

namespace pacsnake
{
	class Pickup : public forge::SceneObject
	{
		RTTI_DECLARE_CLASS( Pickup, forge::SceneObject );

	public:
		pacsnake::GridPawnID GetPawnID() const
		{
			return m_id;
		}

	protected:
		virtual void OnInit( forge::ObjectInitData& initData ) override;

	private:
		forge::CallbackToken m_updateToken;
		pacsnake::GridPawnID m_id;
	};
}