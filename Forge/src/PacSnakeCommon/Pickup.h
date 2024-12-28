#pragma once
#include "../GameEngine/SceneObject.h"

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
		virtual void OnInit( forge::ObjectInitData& initData ) override;

	private:
		forge::CallbackToken m_updateToken;
	};
}