#pragma once
#include "../ECS/Tag.h"

namespace editor
{
	struct EditorObjectTag : ecs::Tag
	{
		RTTI_DECLARE_STRUCT( EditorObjectTag, ecs::Tag );
	};
}