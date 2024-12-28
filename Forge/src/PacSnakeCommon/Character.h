#pragma once
#include "../GameEngine/SceneObject.h"

namespace pacsnake
{
	class Character : public forge::SceneObject
	{
		RTTI_DECLARE_ABSTRACT_CLASS( Character, forge::SceneObject );
	};
}