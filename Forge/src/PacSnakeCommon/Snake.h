#pragma once
#include "Character.h"

namespace forge
{
	class ObjectInitData;
}

namespace pacsnake
{
	class Snake : public pacsnake::Character
	{
		RTTI_DECLARE_CLASS( Snake, pacsnake::Character );

	public:
		virtual void OnInit( forge::ObjectInitData& initData ) override;
	};
}